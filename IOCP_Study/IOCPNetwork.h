#pragma once
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include "Define.h"
#include "IServer.h"
#include "ClientSession.h"

class IServer;

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

	// 콘솔출력
	void ErrorHandling(const char* message, int64_t errorCode = -1);
	void PrintMessage(const char* message);

private:
	// 서버 종류에 따른 참조
	IServer& server_;

	// 서버 리슨 소켓
	SOCKET listen_socket_ = INVALID_SOCKET; 

	// 클라이언트 정보 구조체 벡터
	std::vector<ClientSession> client_infos_;

	// 최대 클라이언트 수
	const uint32_t max_client_count_ = 100;

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

	// 클라이언트 소켓 IOCP 바인드
	bool BindClientToIOCP(ClientInfo* client_info);

	// Recv
	bool RecvRequest(ClientInfo* client_info);

	// Send
	bool SendRequest(ClientInfo* client_info, char* msg, DWORD bytes_length);

	// worker 쓰레드
	void WorkerThread();

	// accepter 쓰레드
	void AccepterThread();

	// 클라이언트 연결 종료
	void CloseSocket(ClientInfo* client_info, bool isForce = false);

	// 사용하지 않는 클라이언트 반환
	ClientSession* GetEmptyClientInfo();
};

