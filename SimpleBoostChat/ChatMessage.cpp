#include "stdafx.h"
#include "ChatMessage.h"
#include <iomanip>




namespace chat {

	std::string Basic_Message::string() const
	{
		return msg.substr(header_size);
	}

	void Basic_Message::create_header()
	{
		std::ostringstream ss,tt;
		ss << std::hex << msg.size();
		tt << std::setfill('0') << std::setw(4) << ss.str();
		msg = tt.str() + msg;

	}

	void Basic_Message::load_message(std::string s)
	{
		msg = std::move(s);
	}

	bool Basic_Message::parse_header()
	{
		std::istringstream ss(msg.substr(0, Basic_Message::header_size));
		ss >> std::hex >> m_body_size;
		if (ss.fail())
			return false;
		resize_msg(header_size + m_body_size);
		return true;

	}

	void ChatMessage::load_message(std::string n, std::string b)
	{
		nick_size = n.size();
		nick = n;
		std::ostringstream ss, msg_buf;
		ss << std::hex << nick_size;
		msg_buf << ss.str() << n << b;
		msg = msg_buf.str();
		create_header();
		parse_message();
	}

	bool ChatMessage::parse_message()
	{
		std::istringstream ss(msg.substr(4, 1));
		ss >> std::hex >> nick_size;
		if (ss.fail())
			return false;
		nick = msg.substr(5, nick_size);
		return true;
	}

	std::string ChatMessage::string()
	{
		return nick + ": " + msg.substr(5 + nick_size);
	}

	/*std::ostream& operator <<(std::ostream& out, const Basic_Message& b)
	{
		out << b.string();
		return out;
	}*/

} // chat namespace