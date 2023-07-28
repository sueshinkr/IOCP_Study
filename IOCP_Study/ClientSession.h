#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdint>
#include "Define.h"

class ClientSession
{
public:
	ClientSession();
	~ClientSession();

	void InitClient(HANDLE iocp_handle, SOCKET client_socket);
	void DisconnectClient(bool isForce = false);

	SOCKET		GetSocket() { return client_socket_; }
	uint32_t	GetIndex() { return client_index_; }
	char*		GetRecvBuf() { return recv_buf_; }

private:
	uint32_t		client_index_;
	SOCKET			client_socket_;
	OverlappedEx	recv_overlapped_ex_;
	OverlappedEx	send_overlapped_ex_;

	char			recv_buf_[kMaxBufSize];
	char			send_buf_[kMaxBufSize];

	bool BindClientToIOCP(HANDLE iocp_handle);
	bool RecvRequest();
	bool SendRequest();

};

