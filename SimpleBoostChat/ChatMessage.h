#pragma once
#include <string>
#include <sstream>
#include <boost/asio.hpp>


/*
	ChatMessage consists of:
	- 4 bytes of hex value consisting number of message chars(leading byte is hex number of nickname chars)
	- accordingly to previous lenght, the nick name
	- message body


*/



/*
	basic message containing string or fixed size byte array 
	every message has fixed size header containing message's body size in hex format
	message body should be of max FFFF characters and header size is fixed to 4 with fillment of '0'
	functions:
	bool parse_header() // converts hex into body size variable
	char* body_data() // gets pointer to data omitting header
	virtual void parse_message() = 0; // mandatory implementation for deriving class, still can be left empty
	size_t body_size()
	protected:
	void reserve_size(size_t n) // resizes container to n bytes


*/


namespace chat {

	class Basic_Message
	{
	public:
		inline static constexpr size_t header_size = 4;
		
	public:
		Basic_Message() : msg("    ") {}

		template<typename U>
		Basic_Message(U&& str, typename std::enable_if_t<std::is_constructible<std::string, U>::value>* = 0) : msg(std::forward<U>(str)) {}
		
		Basic_Message(const Basic_Message&) = default;
		Basic_Message(Basic_Message&&) = default;

		virtual ~Basic_Message() {}

		

		virtual bool parse_message() { return true; }
		virtual bool parse_header();
		virtual std::string string() const;
	 	
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
		using Basic_Message::Basic_Message;
		virtual bool parse_message() final;
		virtual std::string string() const final;
	private:
		std::string nick;
		size_t nick_size;

	};
	

}

