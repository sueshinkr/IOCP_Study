#include "ClientSession.h"

ClientSession::ClientSession()
{
	client_socket_ = INVALID_SOCKET;
	ZeroMemory(&recv_overlapped_ex_, sizeof(recv_overlapped_ex_));
	ZeroMemory(recv_buf_, sizeof(recv_buf_));
}

// 클라이언트 초기화
void ClientSession::InitClient(HANDLE iocp_handle, SOCKET client_socket)
{
	client_socket_ = client_socket;

	// IOCP에 클라이언트 등록
	BindClientToIOCP(iocp_handle);

	// RECV 요청
	RecvRequest();
}

// 클라이언트 연결 종료
void ClientSession::DisconnectClient(bool isForce)
{
	struct linger stLinger = { 0, 0 };

	// 데이터 송수신 중단
	shutdown(client_socket_, SD_BOTH);

	// 소켓 강제 종료 여부 결정
	if (isForce == true) {
		stLinger.l_onoff = 1;
	}

	setsockopt(client_socket_, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	// 소켓 닫기
	closesocket(client_socket_);

	// 클라이언트 정보 구조체 초기화
	client_socket_ = INVALID_SOCKET;
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
	// 아래 두개 변수는 과연 필요한가?
	DWORD dw_flags = 0;
	DWORD dw_number_of_byte_recvd = 0;

	// OverlappedEx 구조체 세팅
	recv_overlapped_ex_.wsa_buf.len = kMaxBufSize;
	recv_overlapped_ex_.wsa_buf.buf = recv_buf_;
	recv_overlapped_ex_.operation = IOOperation::RECV;

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
bool ClientSession::SendRequest(char* data, DWORD data_size)
{
	DWORD dw_flags = 0;
	DWORD dw_number_of_byte_sent = 0;

	// OverlappedEx 구조체 세팅
	OverlappedEx* send_overlapped_ex = new OverlappedEx();

	send_overlapped_ex->wsa_buf.len = data_size;
	send_overlapped_ex->wsa_buf.buf = new char[data_size];
	CopyMemory(send_overlapped_ex->wsa_buf.buf, data, data_size);
	send_overlapped_ex->operation = IOOperation::SEND;

	if (WSASend(client_socket_,
		&(send_overlapped_ex->wsa_buf),
		1,
		&dw_number_of_byte_sent, dw_flags,
		(LPWSAOVERLAPPED) & (send_overlapped_ex),
		NULL) == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {
		ErrorHandling("WSASend() Error!", WSAGetLastError());
		return false;
	}

	return true;
}