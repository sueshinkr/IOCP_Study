#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdint>

// 버퍼 사이즈
const uint32_t kMaxBufSize = 1024;

// 최대 worker 쓰레드 수
const uint16_t kMaxWorkerThread = 4;

// worker 쓰레드에서 사용할 IO 작업 구분
enum class IOOperation
{
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

// 클라이언트 정보 구조체
struct ClientInfo
{
	uint32_t		client_index;
	SOCKET			client_socket;
	OverlappedEx	recv_overlapped_ex;
	OverlappedEx	send_overlapped_ex;

	char			recv_buf[kMaxBufSize];
	char			send_buf[kMaxBufSize];

	ClientInfo()
	{
		client_socket = INVALID_SOCKET;
		ZeroMemory(&recv_overlapped_ex, sizeof(recv_overlapped_ex));
		ZeroMemory(&send_overlapped_ex, sizeof(send_overlapped_ex));
		ZeroMemory(recv_buf, sizeof(recv_buf));
		ZeroMemory(send_buf, sizeof(send_buf));
	}
};