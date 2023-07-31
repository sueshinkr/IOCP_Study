#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdint>

// 버퍼 사이즈
const uint32_t kMaxBufSize = 1024;

// 최대 worker 쓰레드 수
const uint16_t kMaxWorkerThread = 4;

// 
const uint64_t kReuseSesionWaitTimesec = 3;

// worker 쓰레드에서 사용할 IO 작업 구분
enum class IOOperation
{
	ACCEPT,
	RECV,
	SEND
};

// OVERLAPPED 구조체 확장
struct OverlappedEx
{
	WSAOVERLAPPED	wsa_overlapped;
	SOCKET			client_socket;
	WSABUF			wsa_buf;
	IOOperation		operation;
};

// 패킷 구조체
struct RequestPacket
{
	//패킷 데이터
	uint32_t client_index_ = 0;
	uint32_t data_size_ = 0;
	char* data_ = nullptr;
	
	RequestPacket() {}

	RequestPacket(const uint32_t client_index, const uint32_t data_size, char* data)
	{
		Set(client_index, data_size, data);
	}
	
	void Set(RequestPacket& packet)
	{
		Set(packet.client_index_, packet.data_size_, packet.data_);

		delete packet.data_;
	}

	void Set(const uint32_t client_index, const uint32_t data_size, char* data)
	{
		client_index_ = client_index;
		data_size_ = data_size;
		data_ = new char[data_size_];
		CopyMemory(data_, data, data_size_);
	}
};
