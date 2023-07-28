#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdint>
#include "Define.h"
#include "Util.h"

class ClientSession
{
public:
	ClientSession();
	~ClientSession() {};

	void InitClient(HANDLE iocp_handle, SOCKET client_socket);
	void DisconnectClient(bool isForce = false);

	bool RecvRequest();
	bool SendRequest(char* data, DWORD data_size);

	SOCKET		GetSocket() { return client_socket_; }
	uint32_t	GetIndex() { return client_index_; }
	char*		GetRecvBuf() { return recv_buf_; }
	void		SetIndex(uint32_t index) { client_index_ = index; }

private:
	uint32_t		client_index_;
	SOCKET			client_socket_;
	OverlappedEx	recv_overlapped_ex_;

	char			recv_buf_[kMaxBufSize];

	bool BindClientToIOCP(HANDLE iocp_handle);
};

