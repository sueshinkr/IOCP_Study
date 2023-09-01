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
#include "thirdparty\RedisClient.hpp"

class RedisManager
{
public:
	RedisManager() = default;
	~RedisManager() = default;

	bool StartRedisProcessor(std::string ip_, uint16_t port_, int16_t thread_count_);
	void StopRedisProcessor();
	void EnqueueRedisRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data);
	std::optional<PacketInfo> DequeueRedisResponsePacket();

private:
	// 레디스 연결 함수
	bool Connect(std::string ip_, uint16_t port_);
	
	// 레디스 작업 쓰레드 함수
	void RedisProcessorThread();

	void RegisterRedisProcessFunction();
	std::optional<PacketInfo> DequeueRedisRequestPacket();
 	void EnqueueRedisResponsePacket(const uint32_t client_index, const uint32_t data_size, char* data);
	void ProcessPacket(PacketInfo packet);

	// 레디스 작업 함수들
	void ProcessLoginRequest(uint32_t user_index, uint32_t packet_size, char* packet);

	// 레디스 객체
	CRedisClient redis;

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

