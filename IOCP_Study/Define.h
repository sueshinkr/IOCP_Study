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
struct PacketInfo
{
	//패킷 데이터
	uint32_t client_index_ = 0;
	uint16_t packet_id_ = 0;
	uint32_t data_size_ = 0;
	char* data_ = nullptr;
	
	PacketInfo() {}

	PacketInfo(const uint32_t client_index, const uint16_t packet_id, 
				  const uint32_t data_size, char* data)
	{
		Set(client_index, packet_id, data_size, data);
	}
	
	void Set(PacketInfo& packet)
	{
		Set(packet.client_index_, packet.packet_id_, packet.data_size_, packet.data_);

		delete packet.data_;
	}

	void Set(const uint32_t client_index, const uint16_t packet_id,
			 const uint32_t data_size, char* data)
	{
		client_index_ = client_index;
		packet_id_ = packet_id;
		data_size_ = data_size;
		data_ = new char[data_size_];
		CopyMemory(data_, data, data_size_);
	}
};

#pragma pack(push,1)
struct PacketHeader
{
	uint16_t packet_size_;
	uint16_t packet_id_;
	uint8_t  packet_type_;
};

const int MAX_USER_ID_LEN = 32;
const int MAX_USER_PW_LEN = 32;


struct LoginRequestPacket : public PacketHeader
{
	char user_id_[MAX_USER_ID_LEN + 1];
	char user_pw_[MAX_USER_PW_LEN + 1];
};

struct LoginResponsePacket : public PacketHeader
{
	uint16_t result_;
};

struct LoginDBRequestPacket : public PacketHeader
{
	char user_id_[MAX_USER_ID_LEN + 1];
	char user_pw_[MAX_USER_PW_LEN + 1];
};

struct LoginDBResponsePacket : public PacketHeader
{
	char user_id_[MAX_USER_ID_LEN + 1];
	uint16_t result_;
};
#pragma pack(pop)

enum class  PacketId : uint16_t
{
	//SYSTEM
	kSysUserConnect = 0,
	kSysUserDisconnect = 12,
	kSysEnd = 30,

	//DB
	DB_END = 199,

	//Client
	kLoginRequest = 201,
	kLoginResponse = 202,
	kLoginDBRequest = 203,
	kLoginDBResponse = 204,

	ROOM_ENTER_REQUEST = 206,
	ROOM_ENTER_RESPONSE = 207,

	ROOM_LEAVE_REQUEST = 215,
	ROOM_LEAVE_RESPONSE = 216,

	ROOM_CHAT_REQUEST = 221,
	ROOM_CHAT_RESPONSE = 222,
	ROOM_CHAT_NOTIFY = 223,

	//REDIS
	kRedisLogin,
};