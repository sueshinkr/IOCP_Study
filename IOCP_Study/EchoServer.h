#pragma once
#include "IServer.h"

#include <iostream>

#include "IOCPNetwork.h"
#include "PacketManager.h"
#include "UserManager.h"

class EchoServer : public IServer
{
public:
	EchoServer(uint16_t max_client);
	~EchoServer() { }

	void StartServer();
	void StopServer();

	void OnConnect(const uint32_t client_index) override;
	void OnDisconnect(const uint32_t client_index) override;
	void OnReceive(const uint32_t client_index, const uint32_t data_size, char* data) override;
	void OnSend(const uint32_t client_index, const uint32_t data_size) override;

	void InitServerSocket() { iocp_network_->InitServerSocket(); }
	void BindandListenServerSocket(uint16_t port) { iocp_network_->BindandListenServerSocket(port); }

	void SendRequest(uint32_t client_index, uint32_t data_size, char* data);
	
private:
	// IOCP 네트워크
	std::unique_ptr<IOCPNetwork> iocp_network_;

	// 패킷 매니저
	std::unique_ptr<PacketManager> packet_manager_;

	
};

