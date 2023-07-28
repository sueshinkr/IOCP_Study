#include "IOCPNetwork.h"
#include <iostream>

void IOCPNetwork::InitServerSocket()
{
	WSAData wsa_data;

	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0){
		ErrorHandling("WSAStartup() Error!", WSAGetLastError());
	}

	// TCP, Overlapped I/O 소켓 생성
	listen_socket_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_socket_ == INVALID_SOCKET) {
		ErrorHandling("WSASocket() Error!", WSAGetLastError());
	}

	return;
}

void IOCPNetwork::BindandListenServerSocket(uint16_t serverPort)
{
	SOCKADDR_IN server_addr;
	 
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverPort);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 바인딩
	if (bind(listen_socket_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		ErrorHandling("bind() Error!", WSAGetLastError());
	}

	// 리슨
	if (listen(listen_socket_, kBacklogSize) == SOCKET_ERROR) {
		ErrorHandling("listen() Error!", WSAGetLastError());
	}

return;
}

void IOCPNetwork::StartServer()
{
	// 클라이언트 생성
	CreateClient();

	// Completion Port 생성
	// ExistingCompletionPort==null인 경우 NumberOfConcurrentThreads는 무시됨?
	iocp_handle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (iocp_handle_ == NULL) {
		ErrorHandling("CreateIOCPNetwork() Error!", GetLastError());
	}

	// worker 쓰레드 생성
	CreateWorkerThread();

	// accepter 쓰레드 생성
	CreateAccepterThread();
}

void IOCPNetwork::CreateClient()
{
	for (uint32_t i = 0; i < max_client_count_; i++) {
		client_infos_.emplace_back();
	}
}

// worker 쓰레드 생성
void IOCPNetwork::CreateWorkerThread()
{
	for (int i = 0; i < kMaxWorkerThread; i++)
	{
		// 람다도 가능 : worker_threads_.emplace_back([this]() { WokerThread(); });
		worker_threads_.emplace_back(std::thread(&IOCPNetwork::WorkerThread, this));
	}

	if (worker_threads_.size() != kMaxWorkerThread) {
		ErrorHandling("CreateWorkerThread() Error!");
	}
}

// accepter 쓰레드 생성
void IOCPNetwork::CreateAccepterThread()
{
	accepter_thread_ = std::thread(&IOCPNetwork::AccepterThread, this);

	if (accepter_thread_.joinable() == false) {
		ErrorHandling("CreateAccepterThread() Error!");
	}

}

void IOCPNetwork::WorkerThread()
{
	// CompletionKey를 받는 변수
	ClientSession* client_info = nullptr;

	// I/O 작업으로 전송된 데이터 크기를 받는 변수
	DWORD dw_number_of_bytes_transferred = 0;

	// I/O 작업을 위해 요청된 Overlapped 구조체를 받는 변수
	LPOVERLAPPED lp_overlapped = nullptr;

	while (is_worker_run_)
	{
		// I/O 작업 대기
		if (GetQueuedCompletionStatus(iocp_handle_, &dw_number_of_bytes_transferred, (PULONG_PTR)client_info, &lp_overlapped, INFINITE) == false) {
			// I/O 작업 완료가 감지되지 않음
			if (lp_overlapped == NULL) {
				continue;
			}
			else {
				// 클라이언트 접속 끊김 (비정상)
				if (dw_number_of_bytes_transferred == 0) {
					PrintMessage("Client Disconnected!");
					client_info->DisconnectClient();
					continue;
				}
			}
		}
		else {
			// 직접 사용자 쓰레드 종료 메시지 처리 (PostQueuedCompletionStatus() 함수 사용?)
			if (dw_number_of_bytes_transferred == 0 && lp_overlapped == NULL)
			{
				is_worker_run_ = false;
				break;
			}

			// 클라이언트 접속 끊김 (정상)
			if (dw_number_of_bytes_transferred == 0) {
				PrintMessage("Client Disconnected!");
				client_info->DisconnectClient();
				// 연결 종료 알림
				server_.OnDisconnect(client_info->GetIndex());
				continue;
			}
		}

		// 작업 결과에 따른 처리 (RECV / SEND / 예외)

		OverlappedEx* overlapped_ex = reinterpret_cast<OverlappedEx*>(lp_overlapped);

		if (overlapped_ex->operation == IOOperation::RECV) {
			server_.OnReceive(client_info->GetIndex(), dw_number_of_bytes_transferred, client_info->GetRecvBuf());

			// 일단 에코
			SendRequest(client_info, client_info->recv_buf, dw_number_of_bytes_transferred);
		}
		else if (overlapped_ex->operation == IOOperation::SEND) {
			PrintMessage("Send Complete!");
		}
		else {
			// 예외 처리
		}
	}
}

void IOCPNetwork::AccepterThread()
{
	SOCKADDR_IN	client_addr;
	int			client_addr_len = sizeof(client_addr);

	while (is_accepter_run_)
	{
		// 사용하지 않는 클라이언트 정보 구조체 벡터 반환
		ClientSession* client_info = GetEmptyClientInfo();
		if (client_info == nullptr) {
			ErrorHandling("Client Full Error!");
			return;
		}

		// 클라이언트 접속 요청 대기
		SOCKET client_socket = accept(listen_socket_, (SOCKADDR*)&client_addr, &client_addr_len);
		if (client_socket == INVALID_SOCKET) {
			continue;
		}

		// 클라이언트 Init
		client_info->InitClient(iocp_handle_, client_socket);

		// 접속 알림
		server_.OnConnect(client_info->GetIndex());

		// 접속중인 클라이언트 수 증가
		client_count_++;
	}
}

bool IOCPNetwork::BindClientToIOCP(ClientInfo* client_info)
{
	HANDLE iocp_result = CreateIoCompletionPort((HANDLE)client_info->client_socket, iocp_handle_, (ULONG_PTR)client_info, 0);
	if (iocp_result == NULL || iocp_result != iocp_handle_) {
		ErrorHandling("CreateIOCPNetwork() Error!", GetLastError());
		return false;
	}

	return true;
}

void IOCPNetwork::StopServer()
{
	is_worker_run_ = false;

	// IOCP 닫기
	CloseHandle(iocp_handle_);

	// worker thread 종료
	for (auto& thread : worker_threads_) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	
	// accepter thread 종료
	if (accepter_thread_.joinable()) {
		accepter_thread_.join();
	}

	closesocket(listen_socket_);
}

ClientSession* IOCPNetwork::GetEmptyClientInfo()
{
	// 사용하지 않는 클라이언트 반환
	for (auto& client_info : client_infos_)
	{
		if (client_info.GetSocket() == INVALID_SOCKET) {
			return &client_info;
		}
	}

	return nullptr;
}


void IOCPNetwork::ErrorHandling(const char* message, int64_t errorCode)
{
	std::cout << message << std::endl;

	if (errorCode != -1) {
		std::cout << "ErrorCode : " << errorCode << std::endl;
	}

	exit(1);
}

void IOCPNetwork::PrintMessage(const char* message)
{
	std::cout << message << std::endl;
}
