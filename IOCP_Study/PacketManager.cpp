#include "PacketManager.h"

void PacketManager::StartPacketProcessor()
{
	CreatePacketProcessorThread();
}

void PacketManager::StopPacketProcessor()
{
	is_packet_processor_run_ = false;

	// 패킷 처리 쓰레드 종료
	if (packet_processor_thread_.joinable()) {
		packet_processor_thread_.join();
	}
}

void PacketManager::RegisterPacketProcessFunction()
{
	// 이런식으로 추가
	packet_process_function_map_[(int)PacketId::kSysUserConnect] = &PacketManager::ProcessUserConnect;
	packet_process_function_map_[(int)PacketId::kSysUserDisconnect] = &PacketManager::ProcessUserDisconnect;
	packet_process_function_map_[(int)PacketId::kLoginRequest] = &PacketManager::ProcessLoginRequest;
}

void PacketManager::CreatePacketProcessorThread()
{
	packet_processor_thread_ = std::thread(&PacketManager::PacketProcessorThread, this);

	if (packet_processor_thread_.joinable() == false) {
		ErrorHandling("CreatePacketProcessorThread() Error!");
	}
}

void PacketManager::PacketProcessorThread()
{
	while (is_packet_processor_run_)
	{
		RequestPacket packet = DequeueRequestPacket();
		if (packet.data_size_ == 0) {
			// 무한으로 돌지않게 재우기
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else {
			//패킷에 따라 처리
			ProcessPacket(packet);

			// 지금은 에코서버니까 일단 Send만
			//iocp_network_->GetClientSession(packet.client_index_)->SendRequest(packet.data_, packet.data_size_);
		}
	}
}

void PacketManager::ProcessPacket(RequestPacket packet)
{
	// packet.packet_id_ 에 따른 함수 호출
	// <int, function> map을 만들어서 packet_id_를 key로 넣고 함수를 value로 넣어서 호출
	// 정렬 필요없으니 unordered_map 사용

	auto iter = packet_process_function_map_.find(packet.packet_id_);
	if (iter != packet_process_function_map_.end()) {
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
	// 유저풀에서 객체 하나 꺼내오기
}

void PacketManager::ProcessUserDisconnect(uint32_t client_index, uint32_t data_size, char* data)
{
	std::cout << "User DisConnected!" << std::endl;
	// 꺼내왔던 유저 객체 초기화
}

void PacketManager::ProcessLoginRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	// 패킷 오류
	if (data_size != sizeof(LoginRequestPacket)) {
		return;
	}

	// 로그인 로직
	auto login_request = reinterpret_cast<LoginRequestPacket*>(data);

	LoginResponsePacket login_response;
	login_response.packet_id_ = (uint16_t)PacketId::kLoginResponse;
	login_response.packet_size_ = sizeof(LoginResponsePacket);
	 
	// 최대 인원수 초과
	if (user_manager_->GetCurrentUserCount() >= user_manager_->GetMaxUserCount()) {
		login_response.result = -1; // 추후 에러코드 처리
	}
	// 중복 로그인
	else if (user_manager_->GetUserByID(login_request->user_id_) == nullptr) {
		login_response.result = -2; // 추후 에러코드 처리
	}
	// 정상 로그인
	else {
		login_response.result = 0;
		user_manager_->AddUser(login_request->user_id_, client_index);
	}

	send_request_(client_index, sizeof(LoginResponsePacket), reinterpret_cast<char*>(&login_response));
}

void PacketManager::EnqueueRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data)
{
	RequestPacket packet(client_index, data_size, data);

	std::lock_guard<std::mutex> lock(packet_queue_mutex_);
	packet_queue_.push_back(packet);
}

RequestPacket PacketManager::DequeueRequestPacket()
{
	RequestPacket packet;

	std::lock_guard<std::mutex> lock(packet_queue_mutex_);
	// 큐에 패킷이 없을시
	if (packet_queue_.empty()) {
		return RequestPacket();
	}

	// 큐에 패킷 존재시 꺼내오기
	packet.Set(packet_queue_.front());

	// 큐에서 꺼내온 패킷 삭제
	packet_queue_.pop_front();

	return packet;
}

PacketManager::PacketManager(uint16_t max_client, SendRequest send_request)
{
	user_manager_ = std::make_unique<UserManager>(max_client);
	send_request_ = send_request;
}
