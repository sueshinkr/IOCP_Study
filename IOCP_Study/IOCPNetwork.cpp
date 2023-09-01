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

	// Completion Port 생성
	// ExistingCompletionPort==null인 경우 NumberOfConcurrentThreads는 무시됨?
	iocp_handle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (iocp_handle_ == NULL) {
		ErrorHandling("CreateIOCPNetwork() Error!", GetLastError());
	}

	// 리슨소켓 iocp에 등록
	HANDLE iocp_result = CreateIoCompletionPort((HANDLE)listen_socket_, iocp_handle_, 0, 0);
	if (iocp_result == NULL || iocp_result != iocp_handle_) {
		ErrorHandling("CreateIOCPNetwork() Error!", GetLastError());
	}

return;
}

void IOCPNetwork::StartServer()
{
	// 클라이언트 생성
	CreateClient();

	// worker 쓰레드 생성
	CreateWorkerThread();

	// accepter 쓰레드 생성
	CreateAccepterThread();
}

void IOCPNetwork::CreateClient()
{
	client_sessions_.reserve(max_client_count_);

	for (auto i = 0; i < max_client_count_; i++) {
		client_sessions_.emplace_back(i);
		client_sessions_[i].SetIndex(i);

		// 소켓 미리 생성 후 iocp에 등록
		client_sessions_[i].InitSocket(iocp_handle_);
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
	ClientSession* client_session = nullptr;

	// I/O 작업으로 전송된 데이터 크기를 받는 변수
	DWORD dw_number_of_bytes_transferred = 0;

	// I/O 작업을 위해 요청된 Overlapped 구조체를 받는 변수
	LPOVERLAPPED lp_overlapped = nullptr;

	while (is_worker_run_)
	{
		// 하위 항목들 나중에 따로 함수로 빼기...

		// I/O 작업 대기
		if (GetQueuedCompletionStatus(iocp_handle_, &dw_number_of_bytes_transferred, (PULONG_PTR) & client_session, &lp_overlapped, INFINITE) == false) {
			// Completion Port 닫힘
			if (lp_overlapped == NULL) {
				// GetLastError() == ERROR_ABANDONED_WAIT_0
				// 에러처리?
				continue;
			}
			else {
				// 클라이언트 접속 끊김 (비정상)
				if (dw_number_of_bytes_transferred == 0) {
					PrintMessage("Client Disconnected!");
					client_session->DisconnectClient();

					client_count_--;
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

			OverlappedEx* overlapped_ex = reinterpret_cast<OverlappedEx*>(lp_overlapped);

			// 클라이언트 접속 끊김 (정상)
			if (dw_number_of_bytes_transferred == 0 && overlapped_ex->operation != IOOperation::ACCEPT) {
				PrintMessage("Client Disconnected!");
				client_session->DisconnectClient();

				// 연결 종료 알림
				server_.OnDisconnect(client_session->GetIndex(), dw_number_of_bytes_transferred,
									 client_session->GetRecvBuf());
				client_count_--;

				continue;
			}

			// 작업 결과에 따른 처리 (ACCEPT / RECV / SEND / 예외)
			client_session = GetClientSession(overlapped_ex->client_index);

			if (overlapped_ex->operation == IOOperation::ACCEPT) {
				// 클라이언트 접속 완료
				client_session->AcceptComplete(iocp_handle_);
				client_count_++;

				// 연결 완료 알림
				server_.OnConnect(client_session->GetIndex(), dw_number_of_bytes_transferred,
								  client_session->GetRecvBuf());
			}
			else if (overlapped_ex->operation == IOOperation::RECV) {
				auto packet = (LoginRequestPacket*)client_session->GetRecvBuf();
				std::cout << "onreceive id : " << packet->user_id_ << std::endl;

				server_.OnReceive(client_session->GetIndex(), dw_number_of_bytes_transferred,
								  client_session->GetRecvBuf());
				client_session->RecvRequest();
			}
			else if (overlapped_ex->operation == IOOperation::SEND) {
				client_session->SendComplete();
				server_.OnSend(client_session->GetIndex(), dw_number_of_bytes_transferred);
			}
			else {
				// 예외 처리
			}
		}
	}
}

void IOCPNetwork::AccepterThread()
{
	while (is_accepter_run_)
	{	
		// 비동기 I/O Accept
		for (auto& client : client_sessions_) {

			uint64_t cur_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

			if (client.IsConnected() == true) {
				continue;
			}
			else if (cur_time - client.GetLatestClosedTimeSec() <= kReuseSesionWaitTimesec) {
				// 연결 끊어진지 얼마 안된 클라이언트는 넘기기
				continue;
			}

			client.AcceptRequest(listen_socket_);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(32));
	}
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

ClientSession* IOCPNetwork::GetClientSession(const uint32_t client_index)
{
	return &client_sessions_[client_index];
}
