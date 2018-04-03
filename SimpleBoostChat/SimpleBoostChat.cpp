// SimpleBoostChat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ChatMessage.h"
#include <iostream>
#include <string>

int main()
{
	ChatMessage<std::string> my_message;
	my_message.SetHeader("0001f");
	my_message.DecodeHeader();
	std::cout << my_message.NickSize() << std::endl;
	std::cout << my_message.MessageData() << std::endl;
	std::cout << my_message.GetMessageString();
	std::cin.get();
    return 0;
}

