#pragma once
#include <cstdint>
#include <deque>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <functional>

#include "Define.h"
#include "UserManager.h"
#include "Util.h"


class PacketManager
{
public:
	using SendRequest = std::function<void(uint32_t, uint32_t, char*)>;
	
	PacketManager(uint16_t max_client, SendRequest send_request);
	~PacketManager() {}

	void StartPacketProcessor();
	void StopPacketProcessor();

	void EnqueueRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data);
	RequestPacket DequeueRequestPacket();

	

private:
	// 유저 매니저
	std::unique_ptr<UserManager> user_manager_;

	// 패킷 큐
	std::deque<RequestPacket> packet_queue_;

	// 패킷 큐 mutex
	std::mutex packet_queue_mutex_;

	// 패킷 처리 쓰레드
	std::thread packet_processor_thread_;

	// 패킷 처리 함수 포인터
	using packet_process_function_ = void(PacketManager::*)(uint32_t, uint32_t, char*);
	
	// 패킷 처리 함수 map
	std::unordered_map<int, packet_process_function_> packet_process_function_map_;

	// send 함수 포인터
	std::function<void(uint32_t, uint32_t, char*)> send_request_;

	// 패킷 처리 함수 등록
	void RegisterPacketProcessFunction();

	// 패킷 처리 쓰레드 동작 플래그
	bool is_packet_processor_run_ = true;

	// 패킷 처리 쓰레드 생성
	void CreatePacketProcessorThread();

	// 패킷 처리 쓰레드
	void PacketProcessorThread();

	// 패킷 처리
	void ProcessPacket(RequestPacket packet);

	// 패킷 처리 함수들
	void ProcessUserConnect(uint32_t client_index, uint32_t data_size, char* data);
	void ProcessUserDisconnect(uint32_t client_index, uint32_t data_size, char* data);
	void ProcessLoginRequest(uint32_t client_index, uint32_t data_size, char* data);
	
	
};