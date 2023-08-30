//#include "RedisManager.h"
//
//bool RedisManager::StartRedisProcessor(std::string ip_, uint16_t port_, int16_t thread_count_)
//{
//	if (Connect(ip_, port_) == false)
//	{
//		printf("Redis ���� ����\n");
//		return false;
//	}
//
//	is_redis_processor_run_ = true;
//
//	for (auto i = 0; i < thread_count_; i++)
//	{
//		redis_processor_threads_.emplace_back([this]() { RedisProcessorThread(); });
//	}
//
//	printf("Redis ���� ��...\n");
//	return true;
//}
//
//void RedisManager::RegisterRedisProcessFunction()
//{
//	// �̷������� �߰�
//	redis_process_function_map_[(int)PacketId::kRedisLogin] = &RedisManager::ProcessLoginRequest;
//}
//
//
//void RedisManager::StopRedisProcessor()
//{
//	is_redis_processor_run_ = false;
//
//	// ���� �۾� ������ ����
//	for (auto& thread : redis_processor_threads_) {
//		if (thread.joinable()) {
//			thread.join();
//		}
//	}
//}
//
//bool RedisManager::Connect(std::string ip_, uint16_t port_)
//{
//	if (mConn.connect(ip_, port_) == false)
//	{
//		std::cout << "connect error " << mConn.getErrorStr() << std::endl;
//		return false;
//	}
//	else
//	{
//		std::cout << "connect success !!!" << std::endl;
//	}
//
//	return true;
//}
//
//void RedisManager::RedisProcessorThread()
//{
//	while (is_redis_processor_run_)
//	{
//		if (auto packet = DequeueRedisRequestPacket(); packet.has_value() != false) {
//			ProcessPacket(*packet);
//			continue;
//		}
//
//		// �������� �����ʰ� ����
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//	}
//}
//
//std::optional<PacketInfo> RedisManager::DequeueRedisRequestPacket()
//{
//	PacketInfo packet;
//
//	std::lock_guard<std::mutex> lock(redis_request_packet_queue_mutex_);
//	// ť�� ��Ŷ�� ������
//	if (redis_request_packet_queue_.empty()) {
//		return std::nullopt;
//	}
//
//	// ť�� ��Ŷ ����� ��������
//	packet.Set(redis_request_packet_queue_.front());
//
//	// ť���� ������ ��Ŷ ����
//	redis_request_packet_queue_.pop_front();
//
//	return packet;
//}
//
//void RedisManager::ProcessPacket(PacketInfo packet)
//{
//	// packet.packet_id_ �� ���� �Լ� ȣ��
//	// <int, function> map�� ���� packet_id_�� key�� �ְ� �Լ��� value�� �־ ȣ��
//	// ���� �ʿ������ unordered_map ���
//
//	if (auto iter = redis_process_function_map_.find(packet.packet_id_);
//		iter != redis_process_function_map_.end()) {
//		// �Լ� ȣ��
//		(this->*(iter->second))(packet.client_index_, packet.data_size_, packet.data_);
//	}
//	else {
//		std::cout << "Unknown Packet ID : " << packet.packet_id_ << std::endl;
//	}
//}
//
//void RedisManager::EnqueueRedisRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data)
//{
//	auto packet_header = reinterpret_cast<PacketHeader*>(data);
//	PacketInfo packet(client_index, packet_header->packet_id_, data_size, data);
//
//	std::lock_guard<std::mutex> lock(redis_request_packet_queue_mutex_);
//	redis_request_packet_queue_.push_back(packet);
//}
//
//
//std::optional<PacketInfo> RedisManager::DequeueRedisResponsePacket()
//{
//	PacketInfo packet;
//
//	std::lock_guard<std::mutex> lock(redis_response_packet_queue_mutex_);
//	// ť�� ��Ŷ�� ������
//	if (redis_response_packet_queue_.empty()) {
//		return std::nullopt;
//	}
//
//	// ť�� ��Ŷ ����� ��������
//	packet.Set(redis_response_packet_queue_.front());
//
//	// ť���� ������ ��Ŷ ����
//	redis_response_packet_queue_.pop_front();
//
//	return packet;
//}
//
//
//void RedisManager::ProcessLoginRequest(uint32_t user_index, uint32_t packet_size, char* packet)
//{
//	// �α����۾�...
//	// ���� redis_response_packet_queue_�� enqueue
//}
//
