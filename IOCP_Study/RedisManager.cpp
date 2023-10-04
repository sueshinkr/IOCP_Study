#include "RedisManager.h"

bool RedisManager::StartRedisProcessor(std::string ip_, uint16_t port_, int16_t thread_count_)
{
	if (Connect(ip_, port_) == false)
	{
		printf("Redis 접속 실패\n");
		return false;
	}

	is_redis_processor_run_ = true;

	for (auto i = 0; i < thread_count_; i++)
	{
		redis_processor_threads_.emplace_back([this]() { RedisProcessorThread(); });
	}

	RegisterRedisProcessFunction();

	std::cout << "[] ...RedisThread is running...\n";
	return true;
}

void RedisManager::RegisterRedisProcessFunction()
{
	redis_process_function_map_[(int)PacketId::kLoginDBRequest] = &RedisManager::ProcessLoginRequest;
	redis_process_function_map_[(int)PacketId::kLogoutDBRequest] = &RedisManager::ProcessLogoutRequest;
}


void RedisManager::StopRedisProcessor()
{
	is_redis_processor_run_ = false;

	// 레디스 작업 쓰레드 종료
	for (auto& thread : redis_processor_threads_) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

bool RedisManager::Connect(std::string ip_, uint16_t port_)
{
	if (redis.Initialize(ip_, port_, 2, 1) == false) {
		std::cout << "redis connect error"<< std::endl;
		return false;
	}
	else {
		std::cout << "[] ...Redis DB Connect Success!\n";
	}

	return true;
}

void RedisManager::RedisProcessorThread()
{
	while (is_redis_processor_run_)
	{
		if (auto packet = DequeueRedisRequestPacket(); packet.has_value() != false) {
			ProcessPacket(*packet);
			continue;
		}

		// 무한으로 돌지않게 재우기
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

std::optional<PacketInfo> RedisManager::DequeueRedisRequestPacket()
{
	PacketInfo packet;

	std::lock_guard<std::mutex> lock(redis_request_packet_queue_mutex_);
	// 큐에 패킷이 없을시
	if (redis_request_packet_queue_.empty()) {
		return std::nullopt;
	}

	// 큐에 패킷 존재시 꺼내오기
	packet.Set(redis_request_packet_queue_.front());

	// 큐에서 꺼내온 패킷 삭제
	redis_request_packet_queue_.pop_front();

	return packet;
}

void RedisManager::ProcessPacket(PacketInfo packet)
{
	// packet.packet_id_ 에 따른 함수 호출
	// <int, function> map을 만들어서 packet_id_를 key로 넣고 함수를 value로 넣어서 호출
	// 정렬 필요없으니 unordered_map 사용

	if (auto iter = redis_process_function_map_.find(packet.packet_id_);
		iter != redis_process_function_map_.end()) {
		// 함수 호출
		(this->*(iter->second))(packet.client_index_, packet.data_size_, packet.data_);
	}
	else {
		std::cout << "redis Unknown Packet ID : " << packet.packet_id_ << std::endl;
	}
}

void RedisManager::EnqueueRedisRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data)
{
	auto packet_header = reinterpret_cast<PacketHeader*>(data);
	PacketInfo packet(client_index, packet_header->packet_id_, packet_header->packet_size_, data);

	std::lock_guard<std::mutex> lock(redis_request_packet_queue_mutex_);
	redis_request_packet_queue_.push_back(packet);
}

void RedisManager::EnqueueRedisResponsePacket(const uint32_t client_index, const uint32_t data_size, char* data)
{
	auto packet_header = reinterpret_cast<PacketHeader*>(data);
	PacketInfo packet(client_index, packet_header->packet_id_, data_size, data);

	std::lock_guard<std::mutex> lock(redis_response_packet_queue_mutex_);
	redis_response_packet_queue_.push_back(packet);
}


std::optional<PacketInfo> RedisManager::DequeueRedisResponsePacket()
{
	PacketInfo packet;

	std::lock_guard<std::mutex> lock(redis_response_packet_queue_mutex_);
	// 큐에 패킷이 없을시
	if (redis_response_packet_queue_.empty()) {
		return std::nullopt;
	}

	// 큐에 패킷 존재시 꺼내오기
	packet.Set(redis_response_packet_queue_.front());

	// 큐에서 꺼내온 패킷 삭제
	redis_response_packet_queue_.pop_front();

	return packet;
}


void RedisManager::ProcessLoginRequest(uint32_t user_index, uint32_t packet_size, char* packet)
{
	auto login_db_request_pkt = reinterpret_cast<LoginDBRequestPacket*>(packet);

	LoginDBResponsePacket login_db_response_pkt;
	login_db_response_pkt.packet_id_ = (int)PacketId::kLoginDBResponse;
	login_db_response_pkt.packet_size_ = sizeof(LoginDBResponsePacket);
	CopyMemory(login_db_response_pkt.user_id_, login_db_request_pkt->user_id_, kMaxUserIdLen + 1);
	login_db_response_pkt.result_ = (uint16_t)ErrorCode::NONE;

	// 레디스에 ID에 해당하는 키가 있는지 확인
	std::string right_pw;
	if (redis.Get(login_db_request_pkt->user_id_, &right_pw) != RC_SUCCESS) {
		login_db_response_pkt.result_ = (uint16_t)ErrorCode::LOGIN_REDIS_ERROR;
	}
	else if (right_pw == "") {
		std::cout << "User " << login_db_request_pkt->user_id_ << " Created\n";
		redis.Set(login_db_request_pkt->user_id_, login_db_request_pkt->user_pw_);
	}
	// 키(ID)에 해당하는 밸류(PW)가 존재하는지 확인
	else if (right_pw.compare(login_db_request_pkt->user_pw_) != 0) {
		std::cout << "Wrong PassWord... Try Again!\n";
		login_db_response_pkt.result_ = (uint16_t)ErrorCode::LOGIN_USER_INVALID_PW;
	}

	EnqueueRedisResponsePacket(user_index, login_db_response_pkt.packet_size_,
		reinterpret_cast<char*>(&login_db_response_pkt));
}

void RedisManager::ProcessLogoutRequest(uint32_t user_index, uint32_t packet_size, char* packet)
{
	auto logout_db_request_pkt = reinterpret_cast<LogoutDBRequestPacket*>(packet);

	LogoutDBResponsePacket logout_db_response_pkt;
	logout_db_response_pkt.packet_id_ = (int)PacketId::kLogoutDBResponse;
	logout_db_response_pkt.packet_size_ = sizeof(LogoutDBResponsePacket);
	CopyMemory(logout_db_response_pkt.user_id_, logout_db_request_pkt->user_id_, kMaxUserIdLen + 1);
	logout_db_response_pkt.result_ = (uint16_t)ErrorCode::NONE;

	std::string right_pw;
	if (redis.Get(logout_db_request_pkt->user_id_, &right_pw) != RC_SUCCESS) {
		logout_db_response_pkt.result_ = (uint16_t)ErrorCode::LOGIN_USER_NOT_FIND;
	}

	EnqueueRedisResponsePacket(user_index, logout_db_response_pkt.packet_size_,
		reinterpret_cast<char*>(&logout_db_response_pkt));
}

