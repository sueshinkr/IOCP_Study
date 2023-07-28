#pragma once
#include "IServer.h"

#include <iostream>

#include "IOCPNetwork.h"
#include "Packet.h"

class EchoServer : public IServer
{
public:
	EchoServer() {
		iocp_network_ = std::make_unique<IOCPNetwork>(*this);
		packet_manager_ = std::make_unique<PacketManager>();
	}
	~EchoServer() { }

	void StartServer();
	void StopServer();

	void OnConnect(const uint32_t client_index) override;
	void OnDisconnect(const uint32_t client_index) override;
	void OnReceive(const uint32_t client_index, const uint32_t data_size, char* data) override;
	void OnSend(const uint32_t client_index, const uint32_t data_size) override;

	void InitServerSocket() { iocp_network_->InitServerSocket(); }
	void BindandListenServerSocket(uint16_t port) { iocp_network_->BindandListenServerSocket(port); }
	

private:
	// IOCP 네트워크
	std::unique_ptr<IOCPNetwork> iocp_network_;

	// 패킷 매니저
	std::unique_ptr<PacketManager> packet_manager_;

	// 패킷 처리 쓰레드
	std::thread packet_processor_thread_;

	// 패킷 처리 쓰레드 동작 플래그
	bool is_packet_processor_run_ = true;

	// 패킷 처리 쓰레드 생성
	void CreatePacketProcessorThread();

	// 패킷 처리 쓰레드
	void PacketProcessorThread();
};

