#pragma once
#include <cstdint>
#include <deque>
#include <mutex>
#include <iostream>

#include "Define.h"

class PacketManager
{
public:
	PacketManager() {}

	~PacketManager() {}

	void EnqueueRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data);
	RequestPacket DequeueRequestPacket();

private:
	// 패킷 큐
	std::deque<RequestPacket> packet_queue_;

	// mutex
	std::mutex packet_queue_mutex_;
};