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
		boost::asio::async_read(socket, read_msg->header_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
		{
			this->handle_header_read(code);
		});
	}
}

void ChatClient::handle_header_read(const boost::system::error_code& error)
{
	if (!error && read_msg->parse_header())
	{
		read_msg->prepare_receive_buffer();
		boost::asio::async_read(socket, read_msg->msg_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
		{
			this->handle_body_read(code);
		});
	}
	else
		do_close();
}

void ChatClient::handle_body_read(const boost::system::error_code& error)
{
	if (!error && read_msg->parse_msg())
	{
		out << *read_msg->get_msg() << std::endl;
		boost::asio::async_read(socket, read_msg->header_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
		{
			this->handle_header_read(code);
		});
	}
	else
		do_close();
}
 
void ChatClient::write(MessagePtr msg)
{
	ios.post([this,&msg]()
	{
		this->do_write(std::move(msg));
	});
}

void ChatClient::do_write(MessagePtr msg)
{
	msg_queue.push(std::move(msg));
	msg_queue.front()->prepare_send_buffer();
	boost::asio::async_write(socket, msg_queue.front()->msg_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
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
			msg_queue.front()->prepare_send_buffer();
			boost::asio::async_write(socket, msg_queue.front()->msg_buffer(), [this](const boost::system::error_code& code, size_t /*bytes transfered */)
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