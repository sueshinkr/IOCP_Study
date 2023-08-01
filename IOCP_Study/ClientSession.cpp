#include "ClientSession.h"

ClientSession::ClientSession(uint32_t index)
{
	client_index_ = index;
	client_socket_ = INVALID_SOCKET;
	is_connected_ = false;
	latest_closed_time_sec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

	ZeroMemory(&recv_overlapped_ex_, sizeof(recv_overlapped_ex_));
	ZeroMemory(&accept_overlapped_ex_, sizeof(accept_overlapped_ex_));
	ZeroMemory(recv_buf_, sizeof(recv_buf_));
	ZeroMemory(accept_buf_, sizeof(accept_buf_));
}

// 클라이언트 소켓 생성
bool ClientSession::InitSocket(HANDLE iocp_handle)
{
	client_socket_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (client_socket_ == INVALID_SOCKET) {
		ErrorHandling("WSASocket() Error!", WSAGetLastError());
		return false;
	}
	return true;
}

// 클라이언트 연결 종료
void ClientSession::DisconnectClient(bool isForce)
{
	struct linger stLinger = { 0, 0 };

	// 데이터 송수신 중단
	shutdown(client_socket_, SD_BOTH);

	// 소켓 재사용?
	ZeroMemory(&accept_overlapped_ex_, sizeof(accept_overlapped_ex_));
	TransmitFile(client_socket_, NULL, 0, 0, (LPOVERLAPPED)&accept_overlapped_ex_, NULL, TF_DISCONNECT | TF_REUSE_SOCKET);

	latest_closed_time_sec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	is_connected_ = false;
	
	/*
	// 소켓 강제 종료 여부 결정
	if (isForce == true) {
		stLinger.l_onoff = 1;
	}

	setsockopt(client_socket_, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	// 소켓 닫기
	closesocket(client_socket_);

	// 클라이언트 정보 구조체 초기화
	Clear();
	client_socket_ = INVALID_SOCKET;
	*/
}

// IOCP에 클라이언트 등록
bool ClientSession::BindClientToIOCP(HANDLE iocp_handle)
{
	HANDLE iocp_result = CreateIoCompletionPort((HANDLE)client_socket_, iocp_handle, (ULONG_PTR)this, 0);
	if (iocp_result == NULL || iocp_result != iocp_handle) {
		ErrorHandling("CreateIOCPNetwork() Error!", GetLastError());
		return false;
	}

	return true;
}

// RECV 요청
bool ClientSession::RecvRequest()
{
	// 이 변수들은 쓰이는데가 있나?
	DWORD dw_flags = 0;
	DWORD dw_number_of_byte_recvd = 0;

	// OverlappedEx 구조체 세팅
	recv_overlapped_ex_.wsa_buf.len = kMaxBufSize;
	recv_overlapped_ex_.wsa_buf.buf = recv_buf_;
	recv_overlapped_ex_.operation = IOOperation::RECV;
	recv_overlapped_ex_.client_index = client_index_;

	if (WSARecv(client_socket_,
		&(recv_overlapped_ex_.wsa_buf),
		1,
		&dw_number_of_byte_recvd, &dw_flags,
		(LPWSAOVERLAPPED) & (recv_overlapped_ex_),
		NULL) == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {
		ErrorHandling("WSARecv() Error!", WSAGetLastError());
		return false;
	}

	return true;
}

// SEND 요청
bool ClientSession::SendRequest(uint32_t data_size, char* data)
{
	// OverlappedEx 구조체 세팅
	OverlappedEx* send_overlapped_ex = new OverlappedEx();
	ZeroMemory(send_overlapped_ex, sizeof(OverlappedEx));

	send_overlapped_ex->wsa_buf.len = data_size;
	send_overlapped_ex->wsa_buf.buf = new char[data_size];
	CopyMemory(send_overlapped_ex->wsa_buf.buf, data, data_size);
	send_overlapped_ex->operation = IOOperation::SEND;
	send_overlapped_ex->client_index = client_index_;

	// 일단 send 큐에 넣기
	std::lock_guard<std::mutex> guard(send_queue_mutex_);
	send_queue_.push_back(send_overlapped_ex);
	
	// 큐 크기가 1일때만 WSASend 호출 (아닌경우 이전 Send가 끝나지 않은것)
	if (send_queue_.size() == 1) {
		SendIO();
	}

	return true;
}

// ACCEPT 요청
bool ClientSession::AcceptRequest(SOCKET listen_socket)
{
	accept_overlapped_ex_.wsa_buf.len = 0;
	accept_overlapped_ex_.wsa_buf.buf = NULL;
	accept_overlapped_ex_.operation = IOOperation::ACCEPT;
	accept_overlapped_ex_.client_index = client_index_;

	if (AcceptEx(listen_socket,
		client_socket_,
		accept_buf_, 
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		NULL,
		(LPWSAOVERLAPPED) & accept_overlapped_ex_) == FALSE) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			ErrorHandling("AcceptEx() Error!", WSAGetLastError());
			return false;
		}
	}
	std::cout << "AcceptRequest() Success!" << std::endl;

	is_connected_ = true;

	return true;
}

// SEND 
bool ClientSession::SendIO()
{
	DWORD dw_flags = 0;
	DWORD dw_number_of_byte_sent = 0;

	OverlappedEx* send_overlapped_ex = send_queue_.front();

	// WSASend
	if (WSASend(client_socket_,
		&(send_overlapped_ex->wsa_buf),
		1,
		&dw_number_of_byte_sent, dw_flags,
		(LPWSAOVERLAPPED)send_overlapped_ex,
		NULL) == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {
		ErrorHandling("WSASend() Error!", WSAGetLastError());
		return false;
	}

	return true;
}

// SEND 완료
void ClientSession::SendComplete()
{
	std::lock_guard<std::mutex> guard(send_queue_mutex_);

	OverlappedEx* send_overlapped_ex = send_queue_.front();

	// 큐에서 하나 빼기
	delete[] send_overlapped_ex->wsa_buf.buf;
	delete send_overlapped_ex;
	send_queue_.pop_front();

	// 큐가 비어있지 않다면 다음 Send 호출
	if (send_queue_.empty() == false) {
		SendIO();
	}
}

void ClientSession::AcceptComplete(HANDLE iocp_handle)
{
	BindClientToIOCP(iocp_handle);
	// RECV 요청
	RecvRequest();
}

