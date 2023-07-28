#pragma once
#include <cstdint>

class IServer
{
public:
	virtual void OnConnect(const uint32_t client_index) = 0;
	virtual void OnDisconnect(const uint32_t client_index) = 0;
	virtual void OnReceive(const uint32_t client_index, const uint32_t data_size, char* data) = 0;
};