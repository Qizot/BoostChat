#include "stdafx.h"
#include "ChatClient.h"


namespace chat {

ChatClient::ChatClient(std::ostream& out, boost::asio::io_service& io_service,
	tcp::resolver::iterator endpoint_iter) : out(out), ios(io_service), socket(io_service)
{
	boost::asio::async_connect(socket, endpoint_iter, [this](const boost::system::error_code& code, boost::asio::ip::tcp::resolver::iterator iterator)
	{
		this->handle_connect(code);
	});
}

void ChatClient::handle_connect(const boost::system::error_code& error)
{
	if (!error)
	{
		boost::asio::async_read(socket, read_msg.header_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
		{
			this->handle_header_read(code);
		});
	}
}

void ChatClient::handle_header_read(const boost::system::error_code& error)
{
	if (!error && read_msg.parse_header())
	{
		boost::asio::async_read(socket, read_msg.body_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
		{
			this->handle_body_read(code);
		});
	}
	else
		do_close();
}

void ChatClient::handle_body_read(const boost::system::error_code& error)
{
	if (!error && read_msg.parse_message())
	{
		out << read_msg.string();
		boost::asio::async_read(socket, read_msg.header_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
		{
			this->handle_header_read(code);
		});
	}
	else
		do_close();
}
 
void ChatClient::write(chat::ChatMessage & msg)
{
	ios.post([this, &msg]()
	{
		this->do_write(msg);
	});
}

void ChatClient::do_write(chat::ChatMessage& msg)
{
	msg_queue.push(std::move(msg));
	boost::asio::async_write(socket, msg_queue.front().message_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
	{
		this->handle_write(code);
	});
}

void ChatClient::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		msg_queue.pop();
		bool waiting_messages = !msg_queue.empty();
		if (waiting_messages)
		{
			boost::asio::async_write(socket, msg_queue.front().message_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
			{
				this->handle_write(code);
			});
		}
	}
	else
		do_close();
}

void ChatClient::close()
{
	ios.post([this]() {
		this->do_close();
	});
}

void ChatClient::do_close()
{
	socket.close();
}


} // chat namespace