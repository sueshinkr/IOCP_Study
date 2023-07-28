#pragma once
#include <cstdint>
#include <deque>

class PacketData
{
public:
	PacketData(const uint32_t client_index, const uint32_t data_size, char* data);

	void SetPacketData(const uint32_t client_index, const uint32_t data_size, char* data);

private:
	//패킷 데이터
	uint32_t client_index_;
	uint32_t data_size_;
	char* data_;
	
	// 패킷 큐
	std::deque<PacketData> packet_queue_;
};