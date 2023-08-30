#include "User.h"

void User::UserLogin(std::string user_id)
{
	user_Id_ = user_id;
}

void User::Clear()
{
	user_index_ = 0;
	user_Id_ = "";
}
