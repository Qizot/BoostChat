#include "stdafx.h"
#include "ChatServer.h"
#include <iostream>



namespace chat
{

	//=============ChatRoom=====================
	void ChatRoom::join(participant_ptr p)
	{
		
		participants.insert(p);
		for (auto msg : messages)
			p->deliver(std::move(msg));
	}

	void ChatRoom::leave(participant_ptr p)
	{
		
		participants.erase(p);
	}

	void ChatRoom::deliver(message_ptr msg)
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
		boost::asio::async_read(sock, read_msg->header_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
		{
			this->handle_header_read(code);
		});
	}

	
	void SessionParticipant::handle_header_read(const boost::system::error_code& error)
	{
		
		if (!error && read_msg->parse_header())
		{
			read_msg->prepare_receive_buffer();
			boost::asio::async_read(sock, read_msg->msg_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
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

			read_msg->parse_msg();
			std::cout << *read_msg->get_msg() << std::endl;
			
			room.deliver(std::move(read_msg));
			read_msg.reset(new ChatMessage);

			boost::asio::async_read(sock, read_msg->header_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
			{
				this->handle_header_read(code);
			});
		}
		else
			room.leave(shared_from_this());
	}


	void SessionParticipant::deliver(message_ptr msg)
	{
		
		messages.push(std::move(msg));
		messages.front()->prepare_send_buffer();
		boost::asio::async_write(sock, messages.front()->msg_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
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
				messages.front()->prepare_send_buffer();
				boost::asio::async_write(sock, messages.front()->msg_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
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
		
		auto new_session = std::make_shared<chat::SessionParticipant>(ios, room);

		acceptor.async_accept(new_session->socket(), [this,new_session](const boost::system::error_code& code)
		{
			this->handle_accept(code, new_session);
		});
	}

	void ChatServer::handle_accept(const boost::system::error_code& error, participant_ptr p)
	{
		
		if (!error)
			p->participate();
		start_accepting();
	}

} // chat namespace
