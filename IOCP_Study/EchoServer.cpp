#include "EchoServer.h"
#include "Packet.h"

void EchoServer::OnConnect(const uint32_t client_index)
{
	std::cout << "Client " << client_index << " connected!" << std::endl;
}

void EchoServer::OnDisconnect(const uint32_t client_index)
{
	std::cout << "Client " << client_index << " disconnected!" << std::endl;
}

void EchoServer::OnReceive(const uint32_t client_index, const uint32_t data_size, char* data)
{
	std::cout << "Client " << client_index << " sent " << data_size << " bytes!" << std::endl;
	
	// 데이터 패킷화 이후 큐에 넣기
	// 여러 쓰레드에서 동시에 진행할 수 있으니 락 필요
	PacketData packet_data(client_index, data_size, data);

	std::lock_guard<std::mutex> lock(packet_queue_mutex_);
	packet_queue_.emplace_back(packet_data);
}
