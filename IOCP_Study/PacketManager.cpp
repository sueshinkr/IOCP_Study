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
	packet_process_function_map_[(int)PACKET_ID::SYS_USER_CONNECT] = &PacketManager::ProcessUserConnect;
	packet_process_function_map_[(int)PACKET_ID::SYS_USER_DISCONNECT] = &PacketManager::ProcessUserDisconnect;
	packet_process_function_map_[(int)PACKET_ID::LOGIN_REQUEST] = &PacketManager::ProcessLoginRequest;
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
	// 유저 정보 초기화
}

void PacketManager::ProcessUserDisconnect(uint32_t client_index, uint32_t data_size, char* data)
{
	std::cout << "User DisConnected!" << std::endl;
}

void PacketManager::ProcessLoginRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	// 로그인 로직
	// 유저 객체에서 이것저것 
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

