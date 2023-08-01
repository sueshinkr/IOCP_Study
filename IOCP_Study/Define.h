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
	uint32_t		client_index;
	WSABUF			wsa_buf;
	IOOperation		operation;
};

// 패킷 구조체
struct RequestPacket
{
	//패킷 데이터
	uint32_t client_index_ = 0;
	uint32_t data_size_ = 0;
	uint16_t packet_id_ = 0;
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

struct PacketHeader
{
	uint16_t packet_id_;
	uint16_t packet_size_;
};

struct LoginRequestPacket : public PacketHeader
{
	std::string user_id_;
	std::string user_pw_;
};

struct LoginResponsePacket : public PacketHeader
{
	uint16_t result;
};

enum class  PacketId : uint16_t
{
	//SYSTEM
	kSysUserConnect = 11,
	kSysUserDisconnect = 12,
	kSysEnd = 30,

	//DB
	DB_END = 199,

	//Client
	kLoginRequest = 201,
	kLoginResponse = 202,

	ROOM_ENTER_REQUEST = 206,
	ROOM_ENTER_RESPONSE = 207,

	ROOM_LEAVE_REQUEST = 215,
	ROOM_LEAVE_RESPONSE = 216,

	ROOM_CHAT_REQUEST = 221,
	ROOM_CHAT_RESPONSE = 222,
	ROOM_CHAT_NOTIFY = 223,
};