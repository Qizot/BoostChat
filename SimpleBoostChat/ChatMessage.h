#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>


/*
	ChatMessage consists of:
	- 4 bytes of hex value consisting number of message chars(leading byte is hex number of nickname chars)
	- accordingly to previous lenght, the nick name
	- message body


*/




namespace chat {

	class Basic_Message
	{
	public:
		inline static constexpr size_t header_size = 4;
		
	public:
		Basic_Message() : msg("    ") {}

		Basic_Message(std::string str) : msg(str) { create_header(); parse_message(); }
		Basic_Message(std::string&& str) : msg(std::move(str)) { create_header(); }

		virtual void load_message(std::string);
		
		Basic_Message(const Basic_Message&) = default;
		Basic_Message(Basic_Message&&) = default;

		virtual ~Basic_Message() {}

		

		virtual bool parse_message() { return false; }
		virtual bool parse_header();
		virtual std::string string() const;
		std::string raw_message() const { return msg; }
	 	
		decltype(auto) header_buffer() { return boost::asio::buffer(msg.data(), header_size); }
		decltype(auto) body_buffer() { return boost::asio::buffer(msg.data() + header_size, m_body_size); }
		decltype(auto) message_buffer() { return boost::asio::buffer(msg.data(), msg.size()); }
		friend std::ostream& operator <<(std::ostream& out, const Basic_Message& b);	
		
	protected:
		inline void resize_msg(size_t n) { msg.resize(n); }
		void create_header();

	protected:
		std::string msg;
		size_t m_body_size;

	};

	//TODO special constructor for ChatMessage to parse nick and message context into msg and create the header

	class ChatMessage : public Basic_Message
	{
	public:
		ChatMessage() : Basic_Message() {}
		ChatMessage(std::string str) : Basic_Message(str) { parse_message(); }

		
		void load_message(std::string, std::string);
		virtual bool parse_message();
		virtual std::string string();
	private:
		std::string nick;
		size_t nick_size;

	};
	

}

