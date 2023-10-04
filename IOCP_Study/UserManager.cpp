#include "UserManager.h"

UserManager::UserManager()
{
	max_user_count_ = kMaxClient;
	user_pool_.reserve(kMaxClient);

	for (int i = 0; i < kMaxClient; i++)
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
	auto res = user_id_index_map_.find(user_id);

	if (res != user_id_index_map_.end()) {
		return user_pool_[(*res).second];
	}

	//if (user_id_index_map_.find(user_id) != user_id_index_map_.end()) {
	//	return user_pool_[user_id_index_map_[user_id]];
	//}
	return nullptr;
}

uint32_t UserManager::GetUserIndexByID(const std::string user_id)
{
	auto res = user_id_index_map_.find(user_id);

	if (res != user_id_index_map_.end()) {
		return (*res).second;
	}

	return -1;
}

void UserManager::AddUser(std::string user_id, uint32_t user_index)
{
	user_pool_[user_index]->UserLogin(user_id);
	user_id_index_map_.insert(std::make_pair(user_id, user_index));

	current_user_count_++;
}

void UserManager::DelUser(User* user)
{
	printf("DelUser\n");
	user_id_index_map_.erase(user->GetUserId());
	user->UserLogout();

	current_user_count_--;
}
