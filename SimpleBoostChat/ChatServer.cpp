#include "stdafx.h"
#include "ChatServer.h"


namespace chat
{

	//=============ChatRoom=====================
	void ChatRoom::join(participant_ptr p)
	{
		participants.insert(p);
		for (auto& msg : messages)
			p->deliver(msg);
	}

	void ChatRoom::leave(participant_ptr p)
	{
		participants.erase(p);
	}

	void ChatRoom::deliver(chat::ChatMessage msg)
	{
		if (messages.size() > messages_limit)
			messages.pop_front();
		messages.push_back(msg);

		for (auto& _participant : participants)
			_participant->deliver(msg);
	}
	//=============SessionParticipant================

	void SessionParticipant::participate()
	{
		room.join(shared_from_this());
		boost::asio::async_read(sock, read_msg.header_buffer(), [this](const boost::system::error_code& code)
		{
			this->handle_header_read(code);
		});
	}

	
	void SessionParticipant::handle_header_read(const boost::system::error_code& error)
	{
		if (!error && read_msg.parse_header())
		{
			boost::asio::async_read(sock, read_msg.body_buffer(), [this](const boost::system::error_code& code)
			{
				this->handle_body_read(code);
			});

		}
		else
			room.leave(shared_from_this());
	}

	void SessionParticipant::handle_body_read(const boost::system::error_code& error)
	{
		if (!error)
		{
			room.deliver(read_msg);
			boost::asio::async_read(sock, read_msg.header_buffer(), [this](const boost::system::error_code& code)
			{
				this->handle_header_read(code);
			});

		}
		else
			room.leave(shared_from_this());
	}


	void SessionParticipant::deliver(chat::ChatMessage msg)
	{
		messages.push(std::move(msg));
		boost::asio::async_write(sock, messages.front().message_buffer(), [this](const boost::system::error_code& code)
		{
			this->handle_write(code);
		});
	}

	void SessionParticipant::handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			messages.pop();
			bool lasting_messages = !messages.empty();
			if (lasting_messages)
			{
				boost::asio::async_write(sock, messages.front().message_buffer(), [this](const boost::system::error_code& code)
				{
					this->handle_write(code);
				});
			}
		}
		else
			room.leave(shared_from_this());
	}




} // chat namespace
