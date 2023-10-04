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
#include "ErrorCode.h"
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
	// ���� ���� �Լ�
	bool Connect(std::string ip_, uint16_t port_);
	
	// ���� �۾� ������ �Լ�
	void RedisProcessorThread();

	void RegisterRedisProcessFunction();
	std::optional<PacketInfo> DequeueRedisRequestPacket();
 	void EnqueueRedisResponsePacket(const uint32_t client_index, const uint32_t data_size, char* data);
	void ProcessPacket(PacketInfo packet);

	// ���� �۾� �Լ���
	void ProcessLoginRequest(uint32_t user_index, uint32_t packet_size, char* packet);
	void ProcessLogoutRequest(uint32_t user_index, uint32_t packet_size, char* packet);

	// ���� ��ü
	CRedisClient redis;

	// ���� �۾� ������
	std::vector<std::thread> redis_processor_threads_;

	// ���� �۾� ������ ���� �÷���
	bool is_redis_processor_run_ = false;

	// ���� ��û ��Ŷ ť
	std::deque<PacketInfo> redis_request_packet_queue_;
	std::mutex redis_request_packet_queue_mutex_;

	// ���� ���� ��Ŷ ť
	std::deque<PacketInfo> redis_response_packet_queue_;
	std::mutex redis_response_packet_queue_mutex_;

	// ��Ŷ ó�� �Լ� ������
	using redis_process_function_ = void(RedisManager::*)(uint32_t, uint32_t, char*);

	// ��Ŷ ó�� �Լ� map
	std::unordered_map<int, redis_process_function_> redis_process_function_map_;
};

