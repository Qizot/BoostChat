#pragma once
#include <cstdint>
#include <string>
#include <cstdlib>
#include <sstream>
#include <limits>
#include <vector>

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

	class BasicMessage
	{
	public:
		inline static constexpr size_t header_size = 4;
		
	public:
		BasicMessage(const std::string&);
		BasicMessage(std::string&&);
		BasicMessage(const BasicMessage&) = default;
		BasicMessage(BasicMessage&&) = default;

		virtual ~BasicMessage() {}

		void reload_header(std::string);

		virtual void parse_message() {}
		virtual bool parse_header();
		virtual std::string string() const;
	 	

		inline size_t body_size() const { return msg.size() - header_size; }
		inline char* data() { return msg.data() + header_size; }
		

	protected:
		inline void resize_msg(size_t n) { msg.resize(n); }


	private:
		std::string msg;
		size_t body_size;

	};

	class ChatMessage : public BasicMessage
	{
	public:
		virtual void parse_message() final;
	private:
		std::string nick;
		size_t nick_size;

	};
	

}

