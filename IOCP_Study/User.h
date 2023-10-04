#pragma once
#include <cstdint>
#include <string>

class User
{
public:
	enum class UserState
	{
		NONE = 0,
		LOGIN = 1,
		ROOM = 2
	};

	User(uint32_t index) : user_index_(index) {}
	~User() {}

	void	Clear();

	void	UserLogin(std::string user_id);
	void	UserLogout();
	void	EnterRoom(uint32_t room_index_);
	void	LeaveRoom();

	uint32_t	GetIndex() { return user_index_; }
	std::string	GetUserId() { return user_Id_; }
	UserState	GetState() { return user_state_; }
	uint32_t	GetRoomIndex() { return room_index_; }

	void	SetIndex(uint32_t index) { user_index_ = index; }

private:


	uint32_t user_index_ = -1;
	std::string user_Id_ = "";

	uint32_t room_index_ = -1;
	
	UserState user_state_ = UserState::NONE;

};

