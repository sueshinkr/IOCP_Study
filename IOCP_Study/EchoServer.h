#pragma once
#include "IServer.h"

#include <iostream>
#include <deque>
#include <mutex>

#include "IOCPNetwork.h"
#include "Packet.h"

class EchoServer : public IServer
{
public:
	EchoServer() { iocp_network_ = std::make_unique<IOCPNetwork>(*this); }
	~EchoServer() { }

	void OnConnect(const uint32_t client_index) override;
	void OnDisconnect(const uint32_t client_index) override;
	void OnReceive(const uint32_t client_index, const uint32_t data_size, char* data) override;

	void InitServerSocket() { iocp_network_->InitServerSocket(); }
	void BindandListenServerSocket(uint16_t port) { iocp_network_->BindandListenServerSocket(port); }
	void StartServer() { iocp_network_->StartServer(); } // 패킷 처리 쓰레드 생성
	void StopServer() { iocp_network_->StopServer(); }

private:
	std::unique_ptr<IOCPNetwork> iocp_network_;

	// 패킷 큐
	std::deque<PacketData> packet_queue_;

	// mutex
	std::mutex packet_queue_mutex_;

	// 패킷 처리 쓰레드

	// 패킷 큐 확인
};

