#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdint>

// 최대 접속 클라이언트 수
const uint16_t kMaxClient = 10000;

// 버퍼 사이즈
const uint32_t kMaxBufSize = 1024;

// 최대 worker 쓰레드 수
const uint16_t kMaxWorkerThread = 4;

const uint64_t kReuseSesionWaitTimesec = 3;

// 아이디, 비번 최대 사이즈
const uint16_t kMaxUserIdLen = 32;
const uint16_t kMaxUserPwLen = 32;

// 룸 최대 갯수
const uint16_t kMaxRoom = 100;

// 룸 최대 수용 인원수
const uint16_t kMaxRoomCapacity = 1000;

// 채팅 메시지 최대 사이즈
const uint16_t kMaxChatMsgSize = 256;



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

struct SystemConnectPacket : public PacketHeader
{
};

struct SystemDisConnectPacket : public PacketHeader
{
};

// 로그인
struct LoginRequestPacket : public PacketHeader
{
	char user_id_[kMaxUserIdLen + 1];
	char user_pw_[kMaxUserPwLen + 1];
};

struct LoginResponsePacket : public PacketHeader
{
	uint16_t result_;
};

struct LoginDBRequestPacket : public PacketHeader
{
	char user_id_[kMaxUserIdLen + 1];
	char user_pw_[kMaxUserPwLen + 1];
};

struct LoginDBResponsePacket : public PacketHeader
{
	char user_id_[kMaxUserIdLen + 1];
	uint16_t result_;
};

// 로그아웃
struct LogoutRequestPacket : public PacketHeader
{
	char user_id_[kMaxUserIdLen + 1];
};

struct LogoutResponsePacket : public PacketHeader
{
	uint16_t result_;
};

struct LogoutDBRequestPacket : public PacketHeader
{
	char user_id_[kMaxUserIdLen + 1];
};

struct LogoutDBResponsePacket : public PacketHeader
{
	char user_id_[kMaxUserIdLen + 1];
	uint16_t result_;
};

// 룸 입장
struct EnterRoomRequestPacket : public PacketHeader
{
	uint32_t room_index_;
};

struct EnterRoomResponsePacket : public PacketHeader
{
	uint16_t result_;
};

// 룸 퇴장
struct LeaveRoomRequestPacket : public PacketHeader
{
};

struct LeaveRoomResponsePacket : public PacketHeader
{
	uint16_t result_;
};

// 룸 채팅

struct RoomChatRequestPacket : public PacketHeader
{
	char msg_[kMaxChatMsgSize + 1] = { 0, };
};

struct RoomChatResponsePacket : public PacketHeader
{
	uint16_t result_;
};

struct RoomChatNotifyPacket : public PacketHeader
{
	char user_Id_[kMaxUserIdLen + 1] = { 0, };
	char msg_[kMaxChatMsgSize + 1] = { 0, };
};

#pragma pack(pop)

enum class  PacketId : uint16_t
{
	//SYSTEM
	kSysUserConnect = 11,
	kSysUserDisconnect = 12,

	//Client
	kLoginRequest = 201,
	kLoginResponse = 202,

	kLogoutRequest = 203,
	kLogoutResponse = 204,

	kEnterRoomRequest = 206,
	kEnterRoomResponse = 207,

	kLeaveRoomRequest = 215,
	kLeaveRoomResponse = 216,

	kRoomChatRequest = 221,
	kRoomChatResponse = 222,
	kRoomChatNotify = 223,

	//DB
	kLoginDBRequest = 501,
	kLoginDBResponse = 502,
	kLogoutDBRequest = 503,
	kLogoutDBResponse = 504,
};