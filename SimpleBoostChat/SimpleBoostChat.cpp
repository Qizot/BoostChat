// SimpleBoostChat.cpp : Defines the entry point for the console application.
//



#include "stdafx.h"

#include <iostream>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include "Utils.h"
#include "ChatClient.h"
#include "ChatServer.h"
#include "ChatMessage.h"
#include "MessageData.h"

using namespace std::chrono_literals;

bool PrettyGetline(std::istream& in , std::string& str, const std::string& welcome)
{
	std::cout << welcome;
	if (std::getline(in, str))
		return true;
	else
		return false;
}

void Client_Version(int argc, CONSTT char** argv)
{
	using boost::asio::ip::tcp;
	try
	{
		

		boost::asio::io_service io_service;
		std::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(argv[1], argv[2]);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		chat::ChatClient c(std::cout, io_service, iterator);

		std::thread t([&]() { io_service.run(); });
		std::this_thread::sleep_for(1000ms);
		std::string line;
		std::string nick(argv[3]);


		std::shared_ptr<chat::ProtocolMessage> user_data(new chat::ProtocolMessage());
		UserData user({ nick,chat::currentDateTime() });
		user_data->register_user(user);
		io_service.post([&]
		{
			c.write(user_data);
		});
		

		try {
			while (std::getline(std::cin,line) && !io_service.stopped())
			{
				if (!line.empty())
				{
					std::shared_ptr<chat::ProtocolMessage> msg(new chat::ProtocolMessage());
					MessageData m_d{ nick, line, chat::currentTime() };
					msg->chat_message(m_d);
					c.write(msg);
				}
			}
		}catch(std::exception& e) { std::cout << e.what(); }
		work.reset();
		c.close();
		t.join();
		std::cout << "Disconnected!" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
	}
}

void Server_Version(int argc, CONSTT char** argv)
{
	using chat_server_ptr = std::shared_ptr<chat::ChatServer>;
	using server_list = std::vector<chat_server_ptr>;

	try
	{
		std::cout << "Running server_version!\n" << std::endl;
		if (argc < 2)
		{
			std::cout << "Usage: chat_server <port> [<port> ...]\n";
			return;
		}

		boost::asio::io_service io_service;
		std::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));

		server_list  servers;
		for (int i = 1; i < argc; ++i)
		{
			using namespace std; // For atoi.
			tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
			chat_server_ptr server(new chat::ChatServer(io_service, endpoint));
			servers.push_back(server);
		}
		for (auto& p : servers)
			p->start_accepting();

		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
	}
}


int main(int argc, char** argv)
{
	
	//TEST_ARGV
	
	if (argc < 2)
	{
		std::cout << "incorrect use of program, too few arguments...\n";
	}
	
	else
	{
		std::string mode = argv[1];
		if (mode == "server")
		{
			if (argc < 3)
			{
				std::cout << "usage: boostchat <mode> <port> [<port> ...]\n";
				std::cin.get();
				return 42;
			}
			Server_Version(argc - 1, argv + 1);

		}
		else if (mode == "client")
		{

			if (argc != 5)
			{
				std::cout << "usage: boostchat <mode> <host> <port> <nickname>\n";
				std::cin.get();
				return 42;
			}
			Client_Version(argc - 1, argv + 1);

		}
		else
			std::cout << "unknown mode, exiting program!" << std::endl;
	}
	
	
	

	std::cout << "press enter to end the program...";
	std::cin.get();
    return 0;
}


