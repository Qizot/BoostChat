#include "stdafx.h"
#include <iostream>
#include "ChatMessage.h"

using namespace std;


//std::string make_string(boost::asio::mutable_buffers_1& buffer)
//{
//	std::size_t s1 = boost::asio::buffer_size(buffer);
//	char* p1 = boost::asio::buffer_cast<char*>(buffer);
//	return string(p1,s1);
//	
//}
//
//int main()
//{
//	chat::BaseMessage msg;
//	vector<uint8_t> vec{ 65,66,67,68 };
//	auto buf = boost::asio::buffer(vec);
//	
//	cout << make_string(buf);
//
//	
//	
//	
//	std::cin.get();
//}