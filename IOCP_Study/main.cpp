#pragma once

#include <iostream>
#include <cstdint>
#include "ChatServer.h"

// 서버 포트 및 최대 접속 클라이언트 수
const uint16_t kServerPort = 11021;

int main()
{
	ChatServer	chat_server;

	std::cout << "=======================================\n";
	std::cout << "------------IOCP Chat Server-----------\n";
	std::cout << "=======================================\n";

	std::cout << "Input 'QUIT' if you want to close Server\n";

	// 소켓 초기화
	chat_server.InitServerSocket();

	// 소켓 바인딩, 리슨
	chat_server.BindandListenServerSocket(kServerPort);

	// 서버 ON
	chat_server.StartServer();

	// 일단대기
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "QUIT")
		{
			break;
		}
	}

	// 서버 OFF
	chat_server.StopServer();
}
