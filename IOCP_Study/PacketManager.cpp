#include "PacketManager.h"

PacketManager::~PacketManager()
{
}

PacketManager::PacketManager(SendRequest send_request)
{
	send_func_ = send_request;
	user_manager_ = std::make_unique<UserManager>();
	room_manager_ = std::make_unique<RoomManager>(send_request);
	redis_manager_ = std::make_unique<RedisManager>();
}

bool PacketManager::StartPacketProcessor()
{
	CreatePacketProcessorThread();

	if (redis_manager_->StartRedisProcessor("127.0.0.1", 6379, 1) == false) {
		return false;
	}

	RegisterPacketProcessFunction();

	return true;
}

void PacketManager::StopPacketProcessor()
{
	is_packet_processor_run_ = false;

	// 패킷 처리 쓰레드 종료
	if (packet_processor_thread_.joinable()) {
		packet_processor_thread_.join();
	}

	redis_manager_->StopRedisProcessor();
}

void PacketManager::RegisterPacketProcessFunction()
{
	packet_process_function_map_[(int)PacketId::kSysUserConnect] = &PacketManager::ProcessUserConnect;
	packet_process_function_map_[(int)PacketId::kSysUserDisconnect] = &PacketManager::ProcessUserDisconnect;

	packet_process_function_map_[(int)PacketId::kLoginRequest] = &PacketManager::ProcessLoginRequest;
	packet_process_function_map_[(int)PacketId::kLoginDBResponse] = &PacketManager::ProcessLoginDBResponse;

	packet_process_function_map_[(int)PacketId::kLogoutRequest] = &PacketManager::ProcessLogoutRequest;
	packet_process_function_map_[(int)PacketId::kLogoutDBResponse] = &PacketManager::ProcessLogoutDBResponse;

	packet_process_function_map_[(int)PacketId::kEnterRoomRequest] = &PacketManager::ProcessEnterRoomRequest;
	packet_process_function_map_[(int)PacketId::kLeaveRoomRequest] = &PacketManager::ProcessLeaveRoomRequest;

	packet_process_function_map_[(int)PacketId::kRoomChatRequest] = &PacketManager::ProcessRoomChatRequest;
}

void PacketManager::CreatePacketProcessorThread()
{
	packet_processor_thread_ = std::thread([this] {PacketProcessorThread(); });

	if (packet_processor_thread_.joinable() == false) {
		ErrorHandling("CreatePacketProcessorThread() Error!");
	}
}

void PacketManager::PacketProcessorThread()
{
	while (is_packet_processor_run_)
	{
		
		if (auto packet = DequeueRequestPacket(); packet.has_value() != false) {
			ProcessPacket(*packet);
			continue;
		}
		if (auto packet = redis_manager_->DequeueRedisResponsePacket(); packet.has_value() != false) {
			ProcessPacket(*packet);
			continue;
		}

		// 무한으로 돌지않게 재우기
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void PacketManager::ProcessPacket(PacketInfo packet)
{
	// packet.packet_id_ 에 따른 함수 호출
	// <int, function> map을 만들어서 packet_id_를 key로 넣고 함수를 value로 넣어서 호출
	// 정렬 필요없으니 unordered_map 사용

	if (auto iter = packet_process_function_map_.find(packet.packet_id_);
		iter != packet_process_function_map_.end()) {
		// 함수 호출
		(this->*(iter->second))(packet.client_index_, packet.data_size_, packet.data_);
	}
	else {
		//std::cout << "Unknown Packet ID : " << packet.packet_id_ << std::endl;
	}
}

void PacketManager::ProcessUserConnect(uint32_t client_index, uint32_t data_size, char* data)
{
	//// 유저풀에서 객체 하나 꺼내온 후 초기화
	//auto p_user = user_manager_->GetUserByIndex(client_index);
	//p_user->Clear();
}

void PacketManager::ProcessUserDisconnect(uint32_t client_index, uint32_t data_size, char* data)
{
	// 꺼내왔던 유저 객체 초기화 및 정보 제거
	auto p_user = user_manager_->GetUserByIndex(client_index);

	if (p_user->GetState() == User::UserState::ROOM) {
		room_manager_->LeaveUser(p_user->GetRoomIndex(), p_user);
		user_manager_->DelUser(p_user);
	}
	else if (p_user->GetState() == User::UserState::LOGIN) {
		user_manager_->DelUser(p_user);
	}
}

void PacketManager::ProcessLoginRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	std::cout << "********Login Request*********\n";

	// 패킷 오류
	if (data_size != sizeof(LoginRequestPacket)) {
		return;
	}

	// 로그인 로직
	auto login_request = reinterpret_cast<LoginRequestPacket*>(data);

	LoginResponsePacket login_response_pkt;
	login_response_pkt.packet_id_ = (uint16_t)PacketId::kLoginResponse;
	login_response_pkt.packet_size_ = sizeof(LoginResponsePacket);
	 
	// 최대 인원수 초과
	if (user_manager_->GetCurrentUserCount() >= user_manager_->GetMaxUserCount()) {
		std::cout << "Sorry, Server is Full...\n";
		login_response_pkt.result_ = (uint16_t)ErrorCode::LOGIN_USER_USED_ALL_OBJ;
	}
	// 중복 로그인
	else if (user_manager_->GetUserByID(login_request->user_id_) != nullptr) {
		std::cout << "Redundant Connection is NOT ALLOWED!\n";
		login_response_pkt.result_ = (uint16_t)ErrorCode::LOGIN_USER_ALREADY;
	}
	// 정상 로그인
	else {
		LoginDBRequestPacket login_db_request_pkt;
		login_db_request_pkt.packet_id_ = (uint16_t)PacketId::kLoginDBRequest;
		login_db_request_pkt.packet_size_ = sizeof(LoginDBRequestPacket);
		CopyMemory(login_db_request_pkt.user_id_, login_request->user_id_, kMaxUserIdLen + 1);
		CopyMemory(login_db_request_pkt.user_pw_, login_request->user_pw_, kMaxUserPwLen + 1);

		// Redis에 확인 요청
		redis_manager_->EnqueueRedisRequestPacket(client_index, sizeof(LoginDBRequestPacket),
												  reinterpret_cast<char*>(&login_db_request_pkt));
		return;
	}

	// 에러시 응답 전송
	send_func_(client_index, sizeof(LoginResponsePacket), reinterpret_cast<char*>(&login_response_pkt));
}

void PacketManager::ProcessLoginDBResponse(uint32_t client_index, uint32_t data_size, char* data)
{
	// 패킷 오류
	if (data_size != sizeof(LoginDBResponsePacket)) {
		return;
	}

	auto login_db_response = reinterpret_cast<LoginDBResponsePacket*>(data);

	if (login_db_response->result_ == 0) {
		if (user_manager_->GetUserIndexByID(login_db_response->user_id_) == -1) {
			user_manager_->AddUser(login_db_response->user_id_, client_index);
			std::cout << "User " << login_db_response->user_id_ << " Login Success!\n";
		}
		else {
			std::cout << "User " << login_db_response->user_id_ << " Already Login\n";
		}
	}

	LoginResponsePacket login_response;
	login_response.packet_id_ = (uint16_t)PacketId::kLoginResponse;
	login_response.packet_size_ = sizeof(LoginResponsePacket);
	login_response.result_ = login_db_response->result_;

	send_func_(client_index, sizeof(LoginResponsePacket), reinterpret_cast<char*>(&login_response));
}

void PacketManager::ProcessLogoutRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	std::cout << "********Logout Request*********\n";

	// 패킷 오류
	if (data_size != sizeof(LogoutRequestPacket)) {
		return;
	}

	// 로그아웃 로직
	auto logout_request = reinterpret_cast<LogoutRequestPacket*>(data);

	LogoutDBRequestPacket logout_db_request_pkt;
	logout_db_request_pkt.packet_id_ = (uint16_t)PacketId::kLogoutDBRequest;
	logout_db_request_pkt.packet_size_ = sizeof(LogoutDBRequestPacket);
	CopyMemory(logout_db_request_pkt.user_id_, logout_request->user_id_, kMaxUserIdLen + 1);

	// Redis에 요청
	redis_manager_->EnqueueRedisRequestPacket(client_index, logout_db_request_pkt.packet_size_,
		reinterpret_cast<char*>(&logout_db_request_pkt));
}

void PacketManager::ProcessLogoutDBResponse(uint32_t client_index, uint32_t data_size, char* data)
{
	// 패킷 오류
	if (data_size != sizeof(LoginDBResponsePacket)) {
		return;
	}

	auto logout_db_response = reinterpret_cast<LogoutDBResponsePacket*>(data);

	LogoutResponsePacket logout_response;
	logout_response.packet_id_ = (uint16_t)PacketId::kLogoutDBResponse;
	logout_response.packet_size_ = sizeof(LogoutResponsePacket);
	logout_response.result_ = logout_db_response->result_;

	if (logout_db_response->result_ == 0) {
		if (user_manager_->GetUserIndexByID(logout_db_response->user_id_) != -1) {
			
			auto p_user = user_manager_->GetUserByIndex(client_index);
			if (p_user->GetState() == User::UserState::ROOM) {
				auto room_number_ = p_user->GetRoomIndex();
				room_manager_->LeaveUser(room_number_, p_user);
			}

			user_manager_->DelUser(p_user);
			std::cout << "User " << logout_db_response->user_id_ << " Logout Success!\n";
		}
		else {
			std::cout << "User is Not logged in\n";
			logout_response.result_ = (uint16_t)ErrorCode::LOGIN_USER_NOT_FIND;
		}
	}

	send_func_(client_index, sizeof(LoginResponsePacket), reinterpret_cast<char*>(&logout_response));
}

void PacketManager::ProcessEnterRoomRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	auto enter_room_request = reinterpret_cast<EnterRoomRequestPacket*>(data);
	auto p_user = user_manager_->GetUserByIndex(client_index);

	EnterRoomResponsePacket enter_room_response;
	enter_room_response.packet_id_ = (uint16_t)PacketId::kEnterRoomResponse;
	enter_room_response.packet_size_ = sizeof(EnterRoomResponsePacket);
	enter_room_response.result_ = (uint16_t)ErrorCode::NONE;

	if (!p_user) {
		std::cout << "Can't find User\n";
		enter_room_response.result_ = (uint16_t)ErrorCode::ROOM_NOT_FIND_USER;
	}
	else if (p_user->GetState() == User::UserState::NONE) {
		std::cout << "Guest User Not Allowed to Enter the Room\n";
		enter_room_response.result_ = (uint16_t)ErrorCode::ROOM_INVALID_USER_STATUS;
	}
	else if (p_user->GetState() == User::UserState::ROOM) {
		std::cout << "User " << p_user->GetUserId() << " already in the Room\n";
		enter_room_response.result_ = (uint16_t)ErrorCode::ENTER_ROOM_ALREADY;
	}
	else {
		enter_room_response.result_ = room_manager_->EnterUser(enter_room_request->room_index_, p_user);
	}

	send_func_(client_index, sizeof(EnterRoomResponsePacket), reinterpret_cast<char*>(&enter_room_response));
}

void PacketManager::ProcessLeaveRoomRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	auto p_user = user_manager_->GetUserByIndex(client_index);

	LeaveRoomResponsePacket leave_room_response;
	leave_room_response.packet_id_ = (uint16_t)PacketId::kLeaveRoomResponse;
	leave_room_response.packet_size_ = sizeof(LeaveRoomResponsePacket);
	leave_room_response.result_ = (uint16_t)ErrorCode::NONE;

	if (!p_user) {
		std::cout << "Can't find User\n";
		leave_room_response.result_ = (uint16_t)ErrorCode::ROOM_NOT_FIND_USER;
	}
	else if (p_user->GetState() == User::UserState::NONE) {
		std::cout << "Guest User Not Allowed to Leave the Room\n";
		leave_room_response.result_ = (uint16_t)ErrorCode::ROOM_INVALID_USER_STATUS;
	}
	else if (p_user->GetState() != User::UserState::ROOM) {
		std::cout << "User " << p_user->GetUserId() << " Not in the Room\n";
		leave_room_response.result_ = (uint16_t)ErrorCode::ROOM_USER_NOT_IN;
	}
	else {
		auto room_index_ = p_user->GetRoomIndex();
		leave_room_response.result_ = room_manager_->LeaveUser(room_index_, p_user);
	}

	send_func_(client_index, sizeof(LeaveRoomResponsePacket), reinterpret_cast<char*>(&leave_room_response));
}


void PacketManager::ProcessRoomChatRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	auto p_user = user_manager_->GetUserByIndex(client_index);

	RoomChatResponsePacket room_chat_response;
	room_chat_response.packet_id_ = (uint16_t)PacketId::kRoomChatResponse;
	room_chat_response.packet_size_ = sizeof(RoomChatResponsePacket);
	room_chat_response.result_ = (uint16_t)ErrorCode::NONE;

	if (!p_user) {
		std::cout << "Can't find User\n";
		room_chat_response.result_ = (uint16_t)ErrorCode::ROOM_NOT_FIND_USER;
	}
	else if (p_user->GetState() == User::UserState::NONE) {
		std::cout << "Guest User Not Allowed to Chat\n";
		room_chat_response.result_ = (uint16_t)ErrorCode::ROOM_INVALID_USER_STATUS;
	}
	else if (p_user->GetState() != User::UserState::ROOM) {
		std::cout << "User " << p_user->GetUserId() << " Not in the Room\n";
		room_chat_response.result_ = (uint16_t)ErrorCode::ROOM_USER_NOT_IN;
	}
	else {
		auto room_index = p_user->GetRoomIndex();
		auto p_room = room_manager_->GetRoomByIndex(room_index);

		if (p_room == nullptr) {
			room_chat_response.result_ = (uint16_t)ErrorCode::ROOM_INVALID_INDEX;
		}
		else {
			auto room_chat_request = reinterpret_cast<RoomChatRequestPacket*>(data);
			p_room->NotifyChat(client_index, p_user->GetUserId().c_str(), room_chat_request->msg_);
		}
	}

	send_func_(client_index, sizeof(RoomChatResponsePacket), reinterpret_cast<char*>(&room_chat_response));
}

void PacketManager::EnqueueRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data)
{
	auto packet_header = reinterpret_cast<PacketHeader*>(data);
	PacketInfo packet(client_index, packet_header->packet_id_, data_size, data);

	std::lock_guard<std::mutex> lock(packet_queue_mutex_);
	packet_queue_.push_back(packet);
}

std::optional<PacketInfo> PacketManager::DequeueRequestPacket()
{
	PacketInfo packet;

	std::lock_guard<std::mutex> lock(packet_queue_mutex_);
	// 큐에 패킷이 없을시
	if (packet_queue_.empty()) {
		return std::nullopt;
	}

	// 큐에 패킷 존재시 꺼내오기
	packet.Set(packet_queue_.front());

	// 큐에서 꺼내온 패킷 삭제
	packet_queue_.pop_front();

	return packet;
}

