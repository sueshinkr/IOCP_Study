#pragma once

#include <cstdint>
#include <string>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <optional>
#include <unordered_map>

#include "Define.h"
#include "..\thirdparty\CRedisConn.h"

class RedisManager
{
public:
	RedisManager() = default;
	~RedisManager() = default;

//	bool StartRedisProcessor(std::string ip_, uint16_t port_, int16_t thread_count_);
//	void StopRedisProcessor();
//	void EnqueueRedisRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data);
//	std::optional<PacketInfo> DequeueRedisResponsePacket();
//
//private:
//	// 레디스 연결 함수
//	bool Connect(std::string ip_, uint16_t port_);
//	
//	// 레디스 작업 쓰레드 함수
//	void RedisProcessorThread();
//
//	void RegisterRedisProcessFunction();
//	std::optional<PacketInfo> DequeueRedisRequestPacket();
//	void ProcessPacket(PacketInfo packet);
//
//	// 레디스 작업 함수들
//	void ProcessLoginRequest(uint32_t user_index, uint32_t packet_size, char* packet);


	bool StartRedisProcessor(std::string ip_, uint16_t port_, int16_t thread_count_)
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

		printf("Redis 동작 중...\n");
		return true;
	}

	

	void StopRedisProcessor()
	{
		is_redis_processor_run_ = false;

		// 레디스 작업 쓰레드 종료
		for (auto& thread : redis_processor_threads_) {
			if (thread.joinable()) {
				thread.join();
			}
		}
	}

	void EnqueueRedisRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data)
	{
		auto packet_header = reinterpret_cast<PacketHeader*>(data);
		PacketInfo packet(client_index, packet_header->packet_id_, data_size, data);

		std::lock_guard<std::mutex> lock(redis_request_packet_queue_mutex_);
		redis_request_packet_queue_.push_back(packet);
	}


	std::optional<PacketInfo> DequeueRedisResponsePacket()
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

		std::cout << "DequeueResponsePacket\n";

		return packet;
	}

private:

	bool Connect(std::string ip_, uint16_t port_)
	{
		if (mConn.connect(ip_, port_) == false)
		{
			std::cout << "connect error " << mConn.getErrorStr() << std::endl;
			return false;
		}
		else
		{
			std::cout << "connect success !!!" << std::endl;
		}

		return true;
	}

	void RedisProcessorThread()
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

	void RegisterRedisProcessFunction()
	{
		// 이런식으로 추가
		redis_process_function_map_[(int)PacketId::kLoginDBRequest] = &RedisManager::ProcessLoginRequest;
	}


	std::optional<PacketInfo> DequeueRedisRequestPacket()
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

		std::cout << "DequeueRequestPacket\n";

		return packet;
	}

	void EnqueueRedisResponsePacket(const uint32_t client_index, const uint32_t data_size, char* data)
	{
		auto packet_header = reinterpret_cast<PacketHeader*>(data);
		PacketInfo packet(client_index, packet_header->packet_id_, data_size, data);

		std::lock_guard<std::mutex> lock(redis_response_packet_queue_mutex_);
		redis_response_packet_queue_.push_back(packet);

		std::cout << "EnqueueResponsePacket\n";
	}

	void ProcessPacket(PacketInfo packet)
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
			std::cout << "Unknown Redis Packet ID : " << packet.packet_id_ << std::endl;
		}
	}

	void ProcessLoginRequest(uint32_t user_index, uint32_t packet_size, char* packet)
	{
		std::cout << "DBLoginRequest\n";
		auto login_db_request_pkt = reinterpret_cast<LoginDBRequestPacket*>(packet);

		LoginDBResponsePacket login_db_response_pkt;
		login_db_response_pkt.packet_id_ = (int)PacketId::kLoginDBResponse;
		login_db_response_pkt.packet_size_ = sizeof(LoginDBResponsePacket);
		CopyMemory(login_db_response_pkt.user_id_, login_db_request_pkt->user_id_, MAX_USER_ID_LEN + 1);
		login_db_response_pkt.result_ = 0;

		// 레디스에 ID에 해당하는 키가 있는지 확인
		std::string right_pw;
		if (mConn.get(login_db_request_pkt->user_id_, right_pw) == false) {
			login_db_response_pkt.result_ = -1; // 에러코드는 추후...
		}

		// 키(ID)에 해당하는 밸류(PW)가 존재하는지 확인
		if (right_pw.compare(login_db_request_pkt->user_pw_) == false) {
			login_db_response_pkt.result_ = -2;
		}

		EnqueueRedisResponsePacket(user_index, login_db_response_pkt.packet_size_,
								  reinterpret_cast<char*>(&login_db_response_pkt));
	}





	// 레디스 객체
	RedisCpp::CRedisConn mConn;

	// 레디스 작업 쓰레드
	std::vector<std::thread> redis_processor_threads_;

	// 레디스 작업 쓰레드 동작 플래그
	bool is_redis_processor_run_ = false;

	// 레디스 요청 패킷 큐
	std::deque<PacketInfo> redis_request_packet_queue_;
	std::mutex redis_request_packet_queue_mutex_;

	// 레디스 응답 패킷 큐
	std::deque<PacketInfo> redis_response_packet_queue_;
	std::mutex redis_response_packet_queue_mutex_;

	// 패킷 처리 함수 포인터
	using redis_process_function_ = void(RedisManager::*)(uint32_t, uint32_t, char*);

	// 패킷 처리 함수 map
	std::unordered_map<int, redis_process_function_> redis_process_function_map_;
};

