#pragma once
#include <boost/asio.hpp>
#include "ChatMessage.h"
#include <deque>
#include <queue>

using boost::asio::ip::tcp;
using MessagePtr = std::shared_ptr<chat::ChatMessage>;
using MessageQueue = std::queue<MessagePtr>;

namespace chat {

class ChatClient
{
public:
	ChatClient(std::ostream& out, boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iter);

	void write(MessagePtr);
	void close();
	

	void handle_connect(const boost::system::error_code&);
	void handle_header_read(const boost::system::error_code&);
	void handle_body_read(const boost::system::error_code&);

	void handle_write(const boost::system::error_code&);

	void do_write(MessagePtr);
	void do_close();

private:
	boost::asio::io_service& ios;
	tcp::socket socket;
	std::unique_ptr<chat::ChatMessage> read_msg;
	MessageQueue msg_queue;
	std::ostream& out;
	

};




}	// chat namespace

