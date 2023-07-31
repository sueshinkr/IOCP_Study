#pragma once
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <cstdint>
#include <deque>
#include <mutex>
#include "Define.h"
#include "Util.h"

class ClientSession
{
public:
	ClientSession(uint32_t index);
	~ClientSession() {};
	ClientSession(const ClientSession& client_session) {}

	bool InitSocket(HANDLE iocp_handle);
	void DisconnectClient(bool isForce = false);

	bool RecvRequest();
	bool SendRequest(char* data, DWORD data_size);
	bool AcceptRequest(SOCKET listen_socket);

	void SendComplete();
	void AcceptComplete(HANDLE iocp_handle, SOCKET client_socket);

	SOCKET		GetSocket() { return client_socket_; }
	uint32_t	GetIndex() { return client_index_; }
	char*		GetRecvBuf() { return recv_buf_; }
	uint64_t	GetLatestClosedTimeSec() { return latest_closed_time_sec; }
	bool		IsConnected() { return is_connected_; }
	void		SetIndex(uint32_t index) { client_index_ = index; }

private:
	uint32_t		client_index_;
	SOCKET			client_socket_;
	OverlappedEx	recv_overlapped_ex_;
	OverlappedEx	accept_overlapped_ex_;

	char			recv_buf_[kMaxBufSize];
	char			accept_buf_[64];	
	bool			is_connected_;
	uint64_t		latest_closed_time_sec;

	// Send 큐
	std::deque<OverlappedEx*> send_queue_;

	// Send 큐 mutex
	std::mutex send_queue_mutex_;

	bool BindClientToIOCP(HANDLE iocp_handle);
	bool SendIO();	
};

