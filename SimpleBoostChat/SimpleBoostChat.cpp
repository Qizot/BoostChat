// SimpleBoostChat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include "ChatClient.h"
#include "ChatServer.h"
#include "ChatMessage.h"

bool PrettyGetline(std::istream& in , std::string& str, const std::string& welcome)
{
	std::cout << welcome;
	if (std::getline(in, str))
		return true;
	else
		return false;
}

void Client_Version(int argc, char** argv)
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

		std::string line;
		std::string nick(argv[3]);
		std::string pre = nick + "> ";
		while (PrettyGetline(std::cin,line, pre))
		{
			chat::ChatMessage msg;
			msg.load_message(nick, line);
			c.write(std::move(msg));
		}
		work.reset();
		c.close();
		t.join();
		std::cout << "Disconnected client without any errors!" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
	}
}

void Server_Version(int argc, char** argv)
{
	using chat_server_ptr = std::shared_ptr<chat::ChatServer>;
	using server_list = std::vector<chat_server_ptr>;

	try
	{
		std::cout << "Running server_version!\n" << __FUNCTION__;
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
	
	if (argc < 2)
	{
		std::cout << "Incorrect use of program, too few arguments...\n";
	}
	else
	{
		std::string mode = argv[1];
		if (mode == "server")
		{
			if (argc < 3)
			{
				std::cout << "Usage: BoostChat <mode> <port> [<port> ...]\n";
				std::cin.get();
				return 42;
			}
			Server_Version(argc - 1, argv + 1);

		}
		else if (mode == "client")
		{

			if (argc != 5)
			{
				std::cout << "Usage: BoostChat <mode> <host> <port <nickname>\n";
				std::cin.get();
				return 42;
			}
			Client_Version(argc - 1, argv + 1);

		}
		else
			std::cout << "Unknown mode, exiting program!" << std::endl;
	}
	
	
	

	std::cout << "Press enter to end the program...";
	std::cin.get();
    return 0;
}

