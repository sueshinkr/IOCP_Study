#pragma once

#include <functional>

#include "UserManager.h"
#include "Define.h"
#include "ErrorCode.h"

class Room
{
public:
	using SendRequest = std::function<void(uint32_t, uint32_t, char*)>;

	Room(uint16_t room_index, SendRequest send_request);
	~Room() = default;

	uint16_t EnterUser(User* user);
	uint16_t LeaveUser(User* leaveUser);
	void NotifyChat(uint32_t client_Index, const char* user_Id, const char* msg);

	uint16_t GetMaxUserCount() { return max_user_capacity_; }
	uint16_t GetCurrentUserCount() { return cur_user_count_; }
	uint16_t GetRoomNumber() { return room_index_; }

private:
	std::list<User*>	user_list_;
	uint16_t			room_index_ = -1;
	uint16_t			max_user_capacity_ = 0;
	uint16_t			cur_user_count_ = 0;

	std::function<void(uint32_t, uint32_t, char*)> send_func_;

	void SendToAllUser(const uint32_t data_size, char* data, const uint32_t passUserIndex, bool exceptMe);

};
