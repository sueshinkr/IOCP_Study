#include "EchoServer.h"

EchoServer::EchoServer(uint16_t max_client)
{
	iocp_network_ = std::make_unique<IOCPNetwork>(*this);
	packet_manager_ = std::make_unique<PacketManager>(max_client, 
		[this](uint32_t client_index, uint32_t data_size, char* data) {
			SendRequest(client_index, data_size, data);
		});
}

bool EchoServer::StartServer()
{
	if (packet_manager_->StartPacketProcessor() == false) {
		return false;
	}

	iocp_network_->StartServer();

	return true;
}

void EchoServer::StopServer()
{
	packet_manager_->StopPacketProcessor();

	iocp_network_->StopServer();
}

void EchoServer::OnConnect(const uint32_t client_index, const uint32_t data_size, char* data)
{
	std::cout << "Client " << client_index << " connected!" << std::endl;

	packet_manager_->EnqueueRequestPacket(client_index, data_size, data);
}

void EchoServer::OnDisconnect(const uint32_t client_index, const uint32_t data_size, char* data)
{
	std::cout << "Client " << client_index << " disconnected!" << std::endl;

	packet_manager_->EnqueueRequestPacket(client_index, data_size, data);
}

void EchoServer::OnReceive(const uint32_t client_index, const uint32_t data_size, char* data)
{
	std::cout << "Client " << client_index << " received " << data_size << " bytes!" << std::endl;

	auto packet = (LoginRequestPacket*)data;
	std::cout << "id : " << packet->user_id_ << std::endl;

	// 데이터 패킷화 이후 큐에 넣기
	// 여러 쓰레드에서 동시에 진행할 수 있으니 락 필요
	packet_manager_->EnqueueRequestPacket(client_index, data_size, data);
}

void EchoServer::OnSend(const uint32_t client_index, const uint32_t data_size)
{
	std::cout << "Client " << client_index << " sent " << data_size << " bytes!" << std::endl;
}

void EchoServer::SendRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	iocp_network_->GetClientSession(client_index)->SendRequest(data_size, data);
}


