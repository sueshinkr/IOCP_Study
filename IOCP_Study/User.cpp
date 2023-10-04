#include "User.h"
#include <iostream>

void User::UserLogin(std::string user_id)
{
	user_state_ = UserState::LOGIN;
	user_Id_ = user_id;
}

void User::UserLogout()
{
	user_state_ = UserState::NONE;
	user_Id_ = "";
}

void User::EnterRoom(uint32_t room_index)
{
	room_index_ = room_index;
	user_state_ = UserState::ROOM;
}

void User::LeaveRoom()
{
	room_index_ = -1;
	user_state_ = UserState::LOGIN;
}

void User::Clear()
{
	user_index_ = 0;
	user_Id_ = "";
}
