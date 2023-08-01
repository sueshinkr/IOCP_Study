#include <iostream>
#include <cstdint>
#include "EchoServer.h"

// 서버 포트 및 최대 접속 클라이언트 수
const uint16_t kServerPort = 11021;
const uint16_t kMaxClient = 100;

int main()
{
	EchoServer	echo_server(kMaxClient);

	// 소켓 초기화
	echo_server.InitServerSocket();

	// 소켓 바인딩, 리슨
	echo_server.BindandListenServerSocket(kServerPort);

	// 서버 ON
	echo_server.StartServer();

	// 일단대기
	std::cout << "Press any KEY to stop server..." << std::endl;
	getchar();

	// 서버 OFF
	echo_server.StopServer();
}



// 3단계 해야할것
// IOCPServer 라이브러리화 = 네트워크와 서버 로직 분리
// EchoServer 만들고 (상속? 포함?) OnConnect, Onclose, OnReceive 가상함수 만들기
// 추가적으로 더 할만한거 - C++스럽게 코드 리팩토링, 로그찍기 > 일단 위에것들 하고나서 코드리뷰 받아본다음 진행


// 4단계
// 데이터 받고나서 해당 패킷에 따른 로직 처리하는 쓰레드를 따로 분리
// 가져온 데이터를 패킷 큐에 넣어서 확인
// send버퍼 동적할당
// 클라이언트 관련 로직 분리


// 5, 6단계
// 1-send 구현 - 큐 형태로


// 7단계
// Accept 비동기화


// 8단계
// 실질적인 로직들 구현
// server에서 생성해 사용하고있는 패킷 쓰레드를 패킷매니저에서 관리하도록 변경
// 유저매니저에서 유저 객체풀 만들어서 사용
// 시스템 패킷과 유저 패킷 구분 << 하지말고 하나의 큐로 처리해보기


// 9단계
// 레디스 적용
// 레디스 쓰레드 별도로 만들어서 비동기로 작동하도록 (패킷쓰레드에서 작동하면 안됨)
// 예외처리 잘해야됨... (ex DB요청하고 비동기로 응답받기 전에 상태가 달라졌을 경우 등)