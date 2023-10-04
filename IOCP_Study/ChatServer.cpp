#include "ChatServer.h"

ChatServer::ChatServer()
{
	iocp_network_ = std::make_unique<IOCPNetwork>(*this);
	packet_manager_ = std::make_unique<PacketManager>( 
		[this](uint32_t client_index, uint32_t data_size, char* data) {
			SendRequest(client_index, data_size, data);
		});
}

bool ChatServer::StartServer()
{
	if (packet_manager_->StartPacketProcessor() == false) {
		return false;
	}

	iocp_network_->StartServer();

	return true;
}

void ChatServer::StopServer()
{
	packet_manager_->StopPacketProcessor();

	iocp_network_->StopServer();
}

void ChatServer::OnConnect(const uint32_t client_index, const uint32_t data_size, char* data)
{
	std::cout << "Client Number [" << client_index << "] Connected with Server!\n";

	SystemConnectPacket system_connect_pkt;
	system_connect_pkt.packet_id_ = (uint16_t)PacketId::kSysUserConnect;
	system_connect_pkt.packet_size_ = sizeof(SystemConnectPacket);

	packet_manager_->EnqueueRequestPacket(client_index, sizeof(SystemConnectPacket), reinterpret_cast<char*>(&system_connect_pkt));
}

void ChatServer::OnDisconnect(const uint32_t client_index, const uint32_t data_size, char* data)
{
	std::cout << "Client Number [" << client_index << "] DisConnected from Server...\n";

	SystemDisConnectPacket system_disconnect_pkt;
	system_disconnect_pkt.packet_id_ = (uint16_t)PacketId::kSysUserDisconnect;
	system_disconnect_pkt.packet_size_ = sizeof(SystemDisConnectPacket);

	packet_manager_->EnqueueRequestPacket(client_index, sizeof(SystemDisConnectPacket), reinterpret_cast<char*>(&system_disconnect_pkt));
}

void ChatServer::OnReceive(const uint32_t client_index, const uint32_t data_size, char* data)
{
	// 데이터 패킷화 이후 큐에 넣기
	// 여러 쓰레드에서 동시에 진행할 수 있으니 락 필요
	packet_manager_->EnqueueRequestPacket(client_index, data_size, data);
}

void ChatServer::OnSend(const uint32_t client_index, const uint32_t data_size)
{
	/*std::cout << "Client " << client_index << " sent " << data_size << " bytes!" << std::endl;*/
}

void ChatServer::SendRequest(uint32_t client_index, uint32_t data_size, char* data)
{
	iocp_network_->GetClientSession(client_index)->SendRequest(data_size, data);
}


