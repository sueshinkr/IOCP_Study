#include "Room.h"

Room::Room(uint16_t room_index, SendRequest send_request)
{
	room_index_ = room_index;
	send_func_ = send_request;
	max_user_capacity_ = kMaxRoomCapacity;
}

uint16_t Room::EnterUser(User* user)
{
	if (cur_user_count_ >= max_user_capacity_) {
		return (uint16_t)ErrorCode::ENTER_ROOM_FULL_USER;
	}

	user_list_.push_back(user);
	++cur_user_count_;

	user->EnterRoom(room_index_);
	return (uint16_t)ErrorCode::NONE;
}

uint16_t Room::LeaveUser(User* user)
{
	bool finduser = false;

	user_list_.remove_if([&finduser, leaveUserId = user->GetUserId()](User*& user) {
			if (leaveUserId == user->GetUserId()) {
				finduser = true;
			}
			return leaveUserId == user->GetUserId();
		});
	if (finduser == false) {
		return (uint16_t)ErrorCode::ROOM_NOT_FIND_USER;
	}

	user->LeaveRoom();
	return (uint16_t)ErrorCode::NONE;
}

void Room::NotifyChat(uint32_t client_Index, const char* user_Id, const char* msg)
{
	RoomChatNotifyPacket room_chat_notify_pkt;
	room_chat_notify_pkt.packet_id_ = (uint16_t)PacketId::kRoomChatNotify;
	room_chat_notify_pkt.packet_size_ = sizeof(RoomChatNotifyPacket);

	CopyMemory(room_chat_notify_pkt.msg_, msg, sizeof(room_chat_notify_pkt.msg_));
	CopyMemory(room_chat_notify_pkt.user_Id_, user_Id, sizeof(room_chat_notify_pkt.user_Id_));
	
	SendToAllUser(sizeof(room_chat_notify_pkt), (char*)&room_chat_notify_pkt, client_Index, false);
}

void Room::SendToAllUser(const uint32_t data_size, char* data, const uint32_t passUserIndex, bool exceptMe)
{
	for (auto p_user : user_list_)
	{
		if (p_user == nullptr) {
			continue;
		}

		if (exceptMe && p_user->GetIndex() == passUserIndex) {
			continue;
		}

		send_func_((uint32_t)p_user->GetIndex(), (uint32_t)data_size, data);
	}
}