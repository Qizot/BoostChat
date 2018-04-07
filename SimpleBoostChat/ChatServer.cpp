#include "stdafx.h"
#include "ChatServer.h"
#include <iostream>

#define PRINT_FUNCTION_NAME std::cout << __FUNCTION__ << std::endl;

namespace chat
{

	//=============ChatRoom=====================
	void ChatRoom::join(participant_ptr p)
	{
		PRINT_FUNCTION_NAME
		participants.insert(p);
		for (auto& msg : messages)
			p->deliver(msg);
	}

	void ChatRoom::leave(participant_ptr p)
	{
		PRINT_FUNCTION_NAME
		participants.erase(p);
	}

	void ChatRoom::deliver(chat::ChatMessage msg)
	{
		PRINT_FUNCTION_NAME
		if (messages.size() > messages_limit)
			messages.pop_front();
		messages.push_back(msg);

		for (auto& _participant : participants)
			_participant->deliver(msg);
	}
	//=============SessionParticipant================

	void SessionParticipant::participate()
	{
		PRINT_FUNCTION_NAME
		room.join(shared_from_this());
		boost::asio::async_read(sock, read_msg.header_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
		{
			this->handle_header_read(code);
		});
	}

	
	void SessionParticipant::handle_header_read(const boost::system::error_code& error)
	{
		PRINT_FUNCTION_NAME
		if (!error && read_msg.parse_header())
		{
			boost::asio::async_read(sock, read_msg.body_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
			{
				this->handle_body_read(code);
			});

		}
		else
			room.leave(shared_from_this());
	}

	void SessionParticipant::handle_body_read(const boost::system::error_code& error)
	{
		PRINT_FUNCTION_NAME
		if (!error)
		{
			read_msg.parse_message();
			std::cout << read_msg.string() << std::endl;
			
			room.deliver(read_msg);
			boost::asio::async_read(sock, read_msg.header_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
			{
				this->handle_header_read(code);
			});
		}
		else
			room.leave(shared_from_this());
	}


	void SessionParticipant::deliver(chat::ChatMessage msg)
	{
		PRINT_FUNCTION_NAME
		messages.push(std::move(msg));
		boost::asio::async_write(sock, messages.front().message_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
		{
			this->handle_write(code);
		});
	}

	void SessionParticipant::handle_write(const boost::system::error_code& error)
	{
		PRINT_FUNCTION_NAME
		if (!error)
		{
			messages.pop();
			bool lasting_messages = !messages.empty();
			if (lasting_messages)
			{
				boost::asio::async_write(sock, messages.front().message_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
				{
					this->handle_write(code);
				});
			}
		}
		else
			room.leave(shared_from_this());
	}
	//================ChatServer=========================

	void ChatServer::start_accepting()
	{
		PRINT_FUNCTION_NAME
		auto new_session = std::make_shared<chat::SessionParticipant>(ios, room);

		acceptor.async_accept(new_session->socket(), [this,new_session](const boost::system::error_code& code)
		{
			this->handle_accept(code, new_session);
		});
	}

	void ChatServer::handle_accept(const boost::system::error_code& error, participant_ptr p)
	{
		PRINT_FUNCTION_NAME
		if (!error)
			p->participate();
		start_accepting();
	}

} // chat namespace
