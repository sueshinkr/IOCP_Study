#include "Packet.h"

PacketData::PacketData(const uint32_t client_index, const uint32_t data_size, char* data)
{
	client_index_ = client_index;
	data_size_ = data_size;
	data_ = data;
}

void PacketData::SetPacketData(const uint32_t client_index, const uint32_t data_size, char* data)
{
	packet_queue_.emplace_back();
}
