#pragma once

struct PacketData
{
	//패킷 데이터
	uint32_t client_index_;
	uint32_t data_size_;
	char* data_;
	
	PacketData(const uint32_t client_index, const uint32_t data_size, char* data)
	{
		client_index_ = client_index;
		data_size_ = data_size;
		data_ = data;
	}

};