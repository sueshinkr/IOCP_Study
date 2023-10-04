#pragma once

#include <iostream>

#include "Define.h"
#include "ErrorCode.h"
#include "Room.h"

class RoomManager
{
public:
	using SendRequest = std::function<void(uint32_t, uint32_t, char*)>;

	RoomManager(SendRequest send_request);
	~RoomManager() = default;

	std::function<void(uint32_t, uint32_t, char*)> send_request_;
	/*std::function<void(UINT32, UINT16, char*)> SendPacketFunc;*/

	Room* GetRoomByIndex(uint16_t room_index);
	
	uint16_t EnterUser(uint16_t room_index, User* user);
	uint16_t LeaveUser(uint16_t room_index, User* user);

	uint32_t GetMaxRoomCount() { return max_room_count_; }

private:
	std::vector<Room*>	room_pool_;
	uint16_t			max_room_count_ = 0;
};

