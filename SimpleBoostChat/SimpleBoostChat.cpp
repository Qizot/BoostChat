// SimpleBoostChat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ChatMessage.h"
#include <iostream>
#include <string>

int main()
{

	chat::ChatMessage message("00FF3ALA kutas lata jak jebana lopata kurwa w dupe chuj na pizde szmacie jabanek");
	message.parse_header();
	auto buffer = message.body_buffer();
	message.parse_message();

	std::cout << message;
	std::cin.get();
    return 0;
}

