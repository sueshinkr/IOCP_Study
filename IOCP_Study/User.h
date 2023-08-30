#pragma once
#include <cstdint>
#include <string>

class User
{
public:
	User(uint32_t index) : user_index_(index) {}
	~User() {}

	void	UserLogin(std::string user_id);

	void	SetIndex(uint32_t index) { user_index_ = index; }

	void	Clear();

	std::string GetUserID() { return user_Id_; }

private:
	uint32_t user_index_ = 0;
	std::string user_Id_ = "";

};

