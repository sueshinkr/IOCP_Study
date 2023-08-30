//#include "RedisManager.h"
//
//bool RedisManager::StartRedisProcessor(std::string ip_, uint16_t port_, int16_t thread_count_)
//{
//	if (Connect(ip_, port_) == false)
//	{
//		printf("Redis 접속 실패\n");
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
//	printf("Redis 동작 중...\n");
//	return true;
//}
//
//void RedisManager::RegisterRedisProcessFunction()
//{
//	// 이런식으로 추가
//	redis_process_function_map_[(int)PacketId::kRedisLogin] = &RedisManager::ProcessLoginRequest;
//}
//
//
//void RedisManager::StopRedisProcessor()
//{
//	is_redis_processor_run_ = false;
//
//	// 레디스 작업 쓰레드 종료
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
//		// 무한으로 돌지않게 재우기
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//	}
//}
//
//std::optional<PacketInfo> RedisManager::DequeueRedisRequestPacket()
//{
//	PacketInfo packet;
//
//	std::lock_guard<std::mutex> lock(redis_request_packet_queue_mutex_);
//	// 큐에 패킷이 없을시
//	if (redis_request_packet_queue_.empty()) {
//		return std::nullopt;
//	}
//
//	// 큐에 패킷 존재시 꺼내오기
//	packet.Set(redis_request_packet_queue_.front());
//
//	// 큐에서 꺼내온 패킷 삭제
//	redis_request_packet_queue_.pop_front();
//
//	return packet;
//}
//
//void RedisManager::ProcessPacket(PacketInfo packet)
//{
//	// packet.packet_id_ 에 따른 함수 호출
//	// <int, function> map을 만들어서 packet_id_를 key로 넣고 함수를 value로 넣어서 호출
//	// 정렬 필요없으니 unordered_map 사용
//
//	if (auto iter = redis_process_function_map_.find(packet.packet_id_);
//		iter != redis_process_function_map_.end()) {
//		// 함수 호출
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
//	// 큐에 패킷이 없을시
//	if (redis_response_packet_queue_.empty()) {
//		return std::nullopt;
//	}
//
//	// 큐에 패킷 존재시 꺼내오기
//	packet.Set(redis_response_packet_queue_.front());
//
//	// 큐에서 꺼내온 패킷 삭제
//	redis_response_packet_queue_.pop_front();
//
//	return packet;
//}
//
//
//void RedisManager::ProcessLoginRequest(uint32_t user_index, uint32_t packet_size, char* packet)
//{
//	// 로그인작업...
//	// 이후 redis_response_packet_queue_에 enqueue
//}
//
