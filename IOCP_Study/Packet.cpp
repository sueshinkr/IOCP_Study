#include "Packet.h"

void PacketManager::EnqueueRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data)
{
	RequestPacket packet(client_index, data_size, data);

	std::lock_guard<std::mutex> lock(packet_queue_mutex_);
	packet_queue_.push_back(packet);
}

RequestPacket PacketManager::DequeueRequestPacket()
{
	RequestPacket packet;

	std::lock_guard<std::mutex> lock(packet_queue_mutex_);
	// 큐에 패킷이 없을시
	if (packet_queue_.empty()) {
		return RequestPacket();
	}

	// 큐에 패킷 존재시 꺼내오기
	packet.Set(packet_queue_.front());

	// 큐에서 꺼내온 패킷 삭제
	packet_queue_.pop_front();

	return packet;
}
