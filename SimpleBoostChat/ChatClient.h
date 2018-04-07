#pragma once
#include <boost/asio.hpp>
#include "ChatMessage.h"
#include <deque>
#include <queue>

using boost::asio::ip::tcp;
using MessageQueue = std::queue<chat::ChatMessage>;

namespace chat {

class ChatClient
{
public:
	ChatClient(std::ostream& out, boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iter);

	
	void write(chat::ChatMessage&);
	void close();
	

	void handle_connect(const boost::system::error_code&);
	void handle_header_read(const boost::system::error_code&);
	void handle_body_read(const boost::system::error_code&);

	void handle_write(const boost::system::error_code&);

	void do_write(chat::ChatMessage&);
	void do_close();

private:
	boost::asio::io_service& ios;
	tcp::socket socket;
	chat::ChatMessage read_msg;
	MessageQueue msg_queue;
	std::ostream& out;
	

};




}	// chat namespace

