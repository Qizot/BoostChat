#include "stdafx.h"
#include "ChatMessage.h"
#include <iostream>




namespace chat {

	std::string Basic_Message::string() const
	{
		return msg.substr(header_size);
	}

	void Basic_Message::create_header()
	{
		throw;
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

	bool ChatMessage::parse_message()
	{
		std::istringstream ss(msg.substr(4, 1));
		ss >> std::hex >> nick_size;
		if (ss.fail())
			return false;
		nick = msg.substr(5, nick_size);
		return true;
	}

	std::string ChatMessage::string() const
	{
		return nick + ": " + msg.substr(5 + nick_size);

	}

	std::ostream& operator <<(std::ostream& out, const Basic_Message& b)
	{
		out << b.string();
		return out;
	}

} // chat namespace