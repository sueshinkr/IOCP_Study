#pragma once

#include <cstdint>
#include <string>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <optional>
#include <unordered_map>

#include "Define.h"
#include "..\thirdparty\CRedisConn.h"

class RedisManager
{
public:
	RedisManager() = default;
	~RedisManager() = default;

//	bool StartRedisProcessor(std::string ip_, uint16_t port_, int16_t thread_count_);
//	void StopRedisProcessor();
//	void EnqueueRedisRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data);
//	std::optional<PacketInfo> DequeueRedisResponsePacket();
//
//private:
//	// ���� ���� �Լ�
//	bool Connect(std::string ip_, uint16_t port_);
//	
//	// ���� �۾� ������ �Լ�
//	void RedisProcessorThread();
//
//	void RegisterRedisProcessFunction();
//	std::optional<PacketInfo> DequeueRedisRequestPacket();
//	void ProcessPacket(PacketInfo packet);
//
//	// ���� �۾� �Լ���
//	void ProcessLoginRequest(uint32_t user_index, uint32_t packet_size, char* packet);


	bool StartRedisProcessor(std::string ip_, uint16_t port_, int16_t thread_count_)
	{
		if (Connect(ip_, port_) == false)
		{
			printf("Redis ���� ����\n");
			return false;
		}

		is_redis_processor_run_ = true;

		for (auto i = 0; i < thread_count_; i++)
		{
			redis_processor_threads_.emplace_back([this]() { RedisProcessorThread(); });
		}

		RegisterRedisProcessFunction();

		printf("Redis ���� ��...\n");
		return true;
	}

	

	void StopRedisProcessor()
	{
		is_redis_processor_run_ = false;

		// ���� �۾� ������ ����
		for (auto& thread : redis_processor_threads_) {
			if (thread.joinable()) {
				thread.join();
			}
		}
	}

	void EnqueueRedisRequestPacket(const uint32_t client_index, const uint32_t data_size, char* data)
	{
		auto packet_header = reinterpret_cast<PacketHeader*>(data);
		PacketInfo packet(client_index, packet_header->packet_id_, data_size, data);

		std::lock_guard<std::mutex> lock(redis_request_packet_queue_mutex_);
		redis_request_packet_queue_.push_back(packet);
	}


	std::optional<PacketInfo> DequeueRedisResponsePacket()
	{
		PacketInfo packet;

		std::lock_guard<std::mutex> lock(redis_response_packet_queue_mutex_);
		// ť�� ��Ŷ�� ������
		if (redis_response_packet_queue_.empty()) {
			return std::nullopt;
		}

		// ť�� ��Ŷ ����� ��������
		packet.Set(redis_response_packet_queue_.front());

		// ť���� ������ ��Ŷ ����
		redis_response_packet_queue_.pop_front();

		std::cout << "DequeueResponsePacket\n";

		return packet;
	}

private:

	bool Connect(std::string ip_, uint16_t port_)
	{
		if (mConn.connect(ip_, port_) == false)
		{
			std::cout << "connect error " << mConn.getErrorStr() << std::endl;
			return false;
		}
		else
		{
			std::cout << "connect success !!!" << std::endl;
		}

		return true;
	}

	void RedisProcessorThread()
	{
		while (is_redis_processor_run_)
		{
			if (auto packet = DequeueRedisRequestPacket(); packet.has_value() != false) {
				ProcessPacket(*packet);
				continue;
			}

			// �������� �����ʰ� ����
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	void RegisterRedisProcessFunction()
	{
		// �̷������� �߰�
		redis_process_function_map_[(int)PacketId::kLoginDBRequest] = &RedisManager::ProcessLoginRequest;
	}


	std::optional<PacketInfo> DequeueRedisRequestPacket()
	{
		PacketInfo packet;

		std::lock_guard<std::mutex> lock(redis_request_packet_queue_mutex_);
		// ť�� ��Ŷ�� ������
		if (redis_request_packet_queue_.empty()) {
			return std::nullopt;
		}

		// ť�� ��Ŷ ����� ��������
		packet.Set(redis_request_packet_queue_.front());

		// ť���� ������ ��Ŷ ����
		redis_request_packet_queue_.pop_front();

		std::cout << "DequeueRequestPacket\n";

		return packet;
	}

	void EnqueueRedisResponsePacket(const uint32_t client_index, const uint32_t data_size, char* data)
	{
		auto packet_header = reinterpret_cast<PacketHeader*>(data);
		PacketInfo packet(client_index, packet_header->packet_id_, data_size, data);

		std::lock_guard<std::mutex> lock(redis_response_packet_queue_mutex_);
		redis_response_packet_queue_.push_back(packet);

		std::cout << "EnqueueResponsePacket\n";
	}

	void ProcessPacket(PacketInfo packet)
	{
		// packet.packet_id_ �� ���� �Լ� ȣ��
		// <int, function> map�� ���� packet_id_�� key�� �ְ� �Լ��� value�� �־ ȣ��
		// ���� �ʿ������ unordered_map ���

		if (auto iter = redis_process_function_map_.find(packet.packet_id_);
			iter != redis_process_function_map_.end()) {
			// �Լ� ȣ��
			(this->*(iter->second))(packet.client_index_, packet.data_size_, packet.data_);
		}
		else {
			std::cout << "Unknown Redis Packet ID : " << packet.packet_id_ << std::endl;
		}
	}

	void ProcessLoginRequest(uint32_t user_index, uint32_t packet_size, char* packet)
	{
		std::cout << "DBLoginRequest\n";
		auto login_db_request_pkt = reinterpret_cast<LoginDBRequestPacket*>(packet);

		LoginDBResponsePacket login_db_response_pkt;
		login_db_response_pkt.packet_id_ = (int)PacketId::kLoginDBResponse;
		login_db_response_pkt.packet_size_ = sizeof(LoginDBResponsePacket);
		CopyMemory(login_db_response_pkt.user_id_, login_db_request_pkt->user_id_, MAX_USER_ID_LEN + 1);
		login_db_response_pkt.result_ = 0;

		// ���𽺿� ID�� �ش��ϴ� Ű�� �ִ��� Ȯ��
		std::string right_pw;
		if (mConn.get(login_db_request_pkt->user_id_, right_pw) == false) {
			login_db_response_pkt.result_ = -1; // �����ڵ�� ����...
		}

		// Ű(ID)�� �ش��ϴ� ���(PW)�� �����ϴ��� Ȯ��
		if (right_pw.compare(login_db_request_pkt->user_pw_) == false) {
			login_db_response_pkt.result_ = -2;
		}

		EnqueueRedisResponsePacket(user_index, login_db_response_pkt.packet_size_,
								  reinterpret_cast<char*>(&login_db_response_pkt));
	}





	// ���� ��ü
	RedisCpp::CRedisConn mConn;

	// ���� �۾� ������
	std::vector<std::thread> redis_processor_threads_;

	// ���� �۾� ������ ���� �÷���
	bool is_redis_processor_run_ = false;

	// ���� ��û ��Ŷ ť
	std::deque<PacketInfo> redis_request_packet_queue_;
	std::mutex redis_request_packet_queue_mutex_;

	// ���� ���� ��Ŷ ť
	std::deque<PacketInfo> redis_response_packet_queue_;
	std::mutex redis_response_packet_queue_mutex_;

	// ��Ŷ ó�� �Լ� ������
	using redis_process_function_ = void(RedisManager::*)(uint32_t, uint32_t, char*);

	// ��Ŷ ó�� �Լ� map
	std::unordered_map<int, redis_process_function_> redis_process_function_map_;
};

