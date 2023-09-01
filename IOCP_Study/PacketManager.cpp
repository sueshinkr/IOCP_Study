#include "PacketManager.h"

PacketManager::~PacketManager()
{
}

PacketManager::PacketManager(uint16_t max_client, SendRequest send_request)
{
	user_manager_ = std::make_unique<UserManager>(max_client);
	redis_manager_ = std::make_unique<RedisManager>();
	send_request_ = send_request;
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
	// 이런식으로 추가
	packet_process_function_map_[(int)PacketId::kSysUserConnect] = &PacketManager::ProcessUserConnect;
	packet_process_function_map_[(int)PacketId::kSysUserDisconnect] = &PacketManager::ProcessUserDisconnect;
	packet_process_function_map_[(int)PacketId::kLoginRequest] = &PacketManager::ProcessLoginRequest;
	packet_process_function_map_[(int)PacketId::kLoginDBResponse] = &PacketManager::ProcessLoginDBResponse;
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
		std::cout << "Unknown Packet ID : " << packet.packet_id_ << std::endl;
	}
}

void PacketManager::ProcessUserConnect(uint32_t client_index, uint32_t data_size, char* data)
{
	std::cout << "User Connected!" << std::endl;
	
	// 유저풀에서 객체 하나 꺼내온 후 초기화
	auto p_user = user_manager_->GetUserByIndex(client_index);
	p_user->Clear();
}

void PacketManager::ProcessUserDisconnect(uint32_t client_index, uint32_t data_size, char* data)
{
	std::cout << "User DisConnected!" << std::endl;
	
	// 꺼내왔던 유저 객체 초기화 및 정보 제거
	auto p_user = user_manager_->GetUserByIndex(client_index);
	user_manager_->DeleteUser(p_user);
}

void PacketManager::ProcessLoginRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	std::cout << "LoginRequestPacket size : " << sizeof(LoginRequestPacket) << std::endl;
	// 패킷 오류
	if (data_size != sizeof(LoginRequestPacket)) {
		//return;
	}

	// 로그인 로직
	auto login_request = reinterpret_cast<LoginRequestPacket*>(data);

	LoginResponsePacket login_response_pkt;
	login_response_pkt.packet_id_ = (uint16_t)PacketId::kLoginResponse;
	login_response_pkt.packet_size_ = sizeof(LoginResponsePacket);
	 
	// 최대 인원수 초과
	if (user_manager_->GetCurrentUserCount() >= user_manager_->GetMaxUserCount()) {
		login_response_pkt.result_ = -1; // 추후 에러코드 처리
	}
	// 중복 로그인
	else if (user_manager_->GetUserByID(login_request->user_id_) != nullptr) {
		login_response_pkt.result_ = -2; // 추후 에러코드 처리
		std::cout << "Login Fail\n";
	}
	// 정상 로그인
	else {

		LoginDBRequestPacket login_db_request_pkt;
		login_db_request_pkt.packet_id_ = (uint16_t)PacketId::kLoginDBRequest;
		login_db_request_pkt.packet_size_ = sizeof(LoginDBRequestPacket);
		CopyMemory(login_db_request_pkt.user_id_, login_request->user_id_, MAX_USER_ID_LEN + 1);
		CopyMemory(login_db_request_pkt.user_pw_, login_request->user_pw_, MAX_USER_PW_LEN + 1);

		// Redis에 확인 요청
		redis_manager_->EnqueueRedisRequestPacket(client_index, sizeof(LoginDBRequestPacket),
												  reinterpret_cast<char*>(&login_db_request_pkt));
		return;
	}

	// 에러시 응답 전송
	send_request_(client_index, sizeof(LoginResponsePacket), reinterpret_cast<char*>(&login_response_pkt));
}

void PacketManager::ProcessLoginDBResponse(uint32_t client_index, uint32_t data_size, char* data)
{
	std::cout << "DBLoginResponse\n";
	// 패킷 오류
	if (data_size != sizeof(LoginDBResponsePacket)) {
		return;
	}

	auto login_db_response = reinterpret_cast<LoginDBResponsePacket*>(data);

	if (login_db_response->result_ == 0) {
		user_manager_->AddUser(login_db_response->user_id_, client_index);
		std::cout << "Login Success\n";
	}

	LoginResponsePacket login_response;
	login_response.packet_id_ = (uint16_t)PacketId::kLoginResponse;
	login_response.packet_size_ = sizeof(LoginResponsePacket);
	login_response.result_ = login_db_response->result_;

	send_request_(client_index, sizeof(LoginResponsePacket), reinterpret_cast<char*>(&login_response));
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

