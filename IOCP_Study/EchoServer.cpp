#include "EchoServer.h"
#include "Packet.h"

void EchoServer::StartServer()
{
	CreatePacketProcessorThread();
	iocp_network_->StartServer();
}

void EchoServer::StopServer()
{
	is_packet_processor_run_ = false;

	// 패킷 처리 쓰레드 종료
	if (packet_processor_thread_.joinable()) {
		packet_processor_thread_.join();
	}

	iocp_network_->StopServer();
}

void EchoServer::CreatePacketProcessorThread()
{
	packet_processor_thread_ = std::thread(&EchoServer::PacketProcessorThread, this);

	if (packet_processor_thread_.joinable() == false) {
		ErrorHandling("CreatePacketProcessorThread() Error!");
	}
}

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
	packet_manager_->EnqueueRequestPacket(client_index, data_size, data);
}

void EchoServer::OnSend(const uint32_t client_index, const uint32_t data_size)
{
	std::cout << "Client " << client_index << " sent " << data_size << " bytes!" << std::endl;
}

void EchoServer::PacketProcessorThread()
{
	while (is_packet_processor_run_)
	{
		RequestPacket packet = packet_manager_->DequeueRequestPacket();
		if (packet.data_size_ == 0) {
			// 무한으로 돌지않게 재우기
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else {
			// 지금은 에코서버니까 일단 Send만
			iocp_network_->GetClientSession(packet.client_index_)->SendRequest(packet.data_, packet.data_size_);
		}
	}

}