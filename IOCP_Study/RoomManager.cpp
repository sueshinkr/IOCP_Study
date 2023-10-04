#include "RoomManager.h"

RoomManager::RoomManager(SendRequest send_request)
{
	max_room_count_ = kMaxRoom;

	room_pool_.reserve(max_room_count_);

	for (auto i = 0; i < max_room_count_; i++)
	{
		room_pool_.push_back(new Room(i, send_request));
	}
}

Room* RoomManager::GetRoomByIndex(uint16_t room_index)
{
	if (room_index < 0 || room_index >= max_room_count_) {
		return nullptr;
	}

	return room_pool_[room_index];
}

uint16_t RoomManager::EnterUser(uint16_t room_index, User* user)
{
	auto p_room = GetRoomByIndex(room_index);
	if (p_room == nullptr) {
		std::cout << "Invalid RoomNumber\n";
		return (uint16_t)ErrorCode::ROOM_INVALID_INDEX;
	}

	std::cout << "User " << user->GetUserId() << " Enter the Room [" << room_index << "]\n";
	return p_room->EnterUser(user);
}

uint16_t RoomManager::LeaveUser(uint16_t room_index, User* user)
{
	auto p_room = GetRoomByIndex(room_index);
	if (p_room == nullptr) {
		return (uint16_t)ErrorCode::ROOM_INVALID_INDEX;
	}

	std::cout << "User " << user->GetUserId() << " Leave the Room [" << room_index << "]\n";
	return (p_room->LeaveUser(user));
}