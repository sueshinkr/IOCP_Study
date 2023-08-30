#pragma once
#include <vector>
#include <unordered_map>

#include "User.h"

class UserManager
{
public:
	UserManager(uint16_t max_client);
	~UserManager();

	User*	GetUserByIndex(const uint32_t user_index);
	User*	GetUserByID(const std::string user_id);
	void	AddUser(std::string user_id, uint32_t user_index);
	void	DeleteUser(User* user);

	uint64_t GetCurrentUserCount() { return current_user_count_; }
	uint64_t GetMaxUserCount() { return max_user_count_; }


private:
	std::vector<User*> user_pool_;
	std::unordered_map<std::string, uint64_t> user_id_index_map_;

	uint64_t current_user_count_ = 0;
	uint64_t max_user_count_ = 0;
};

