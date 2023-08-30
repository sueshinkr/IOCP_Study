#pragma once
#pragma comment(lib, "ws2_32")

#include "IServer.h"
#include "ClientSession.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>

// IOCP 클래스
class IOCPNetwork
{
public:
	// 생성자
	IOCPNetwork(IServer& server) : server_(server) {}

	// 소멸자
	~IOCPNetwork() { WSACleanup(); }

	// 소켓 초기화
	void InitServerSocket();

	// 소켓 바인드, 리슨
	void BindandListenServerSocket(uint16_t port);

	// 서버 가동
	void StartServer();

	// 서버 종료
	void StopServer();

	// 클라이언트 세션 가져오기
	ClientSession* GetClientSession(const uint32_t client_index);

private:
	// 서버 종류에 따른 참조
	IServer& server_;

	// 서버 리슨 소켓
	SOCKET listen_socket_ = INVALID_SOCKET; 

	// 클라이언트 세션 풀
	std::vector<ClientSession> client_sessions_;

	// 최대 클라이언트 수
	const uint32_t max_client_count_ = 5;

	// 접속중인 클라이언트 수
	int32_t client_count_ = 0;

	// IOCP 핸들
	HANDLE iocp_handle_ = INVALID_HANDLE_VALUE;

	// Worker 쓰레드
	std::vector<std::thread> worker_threads_;

	// Worker 쓰레드 동작 플래그
	bool is_worker_run_ = true;

	// Accepter 쓰레드
	std::thread accepter_thread_;

	// Accepter 쓰레드 동작 플래그
	bool is_accepter_run_ = true;

	// 최대 접속 연결 요청 수
	const uint16_t kBacklogSize = 5;

	// 클라이언트 생성
	void CreateClient();

	// worker 쓰레드 생성
	void CreateWorkerThread();

	// accepter 쓰레드 생성
	void CreateAccepterThread();

	// worker 쓰레드
	void WorkerThread();

	// accepter 쓰레드
	void AccepterThread();
};

