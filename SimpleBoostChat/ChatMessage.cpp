#include "stdafx.h"
#include "ChatMessage.h"

chat::BasicMessage::BasicMessage(const std::string& str)
{
	msg = str;
}

chat::BasicMessage::BasicMessage(std::string&& str)
{
	msg = std::move(str);
}

std::string chat::BasicMessage::string() const
{
	return msg.substr(header_size);
}

bool chat::BasicMessage::parse_header()
{
	const char* beg = msg.c_str();
	char* end = msg.data() + 4;
	body_size = std::strtoull(msg.c_str(), &end, 16);
	if (body_size == 0 && msg.substr(0, 4) != "0000")
		return false;
	return true;

}

void chat::ChatMessage::parse_message()
{

}
