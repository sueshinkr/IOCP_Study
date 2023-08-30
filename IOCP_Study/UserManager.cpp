#include "UserManager.h"

UserManager::UserManager(uint16_t max_client)
{
	max_user_count_ = max_client;
	user_pool_.reserve(max_user_count_);

	for (int i = 0; i < max_client; i++)
	{
		user_pool_.push_back(new User(i));
	}
}

UserManager::~UserManager()
{
	for (auto user : user_pool_)
	{
		delete user;
	}
}

User* UserManager::GetUserByIndex(const uint32_t user_index)
{
	return user_pool_[user_index];
}

User* UserManager::GetUserByID(const std::string user_id)
{
	if (user_id_index_map_.find(user_id) != user_id_index_map_.end()) {
		return user_pool_[user_id_index_map_[user_id]];
	}

	return nullptr;
}

void UserManager::AddUser(std::string user_id, uint32_t user_index)
{
	user_pool_[user_index]->UserLogin(user_id);
	user_id_index_map_.insert(std::make_pair(user_id, user_index));

	current_user_count_++;
}

void UserManager::DeleteUser(User* user)
{
	user_id_index_map_.erase(user->GetUserID());
	user->Clear();

	current_user_count_--;
}
