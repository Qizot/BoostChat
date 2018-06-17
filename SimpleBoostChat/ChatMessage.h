#pragma once
#include <nlohmann/json.hpp>
#include <optional>
#include <boost/asio.hpp>
#include "MessageData.h"
#include "Utils.h"

namespace chat {
	class BaseMessage;
	using ChatMessage = BaseMessage;
	using json = nlohmann::json;



	struct ChatException : public std::exception
	{
		std::string err;
		ChatException(std::string a) : err(std::string("ChatException: ") + a) {}
		const char* what() noexcept { return err.c_str(); }
	};

	class BaseMessage
	{
		static constexpr std::size_t HEADER_SIZE = 8;

	public:
		BaseMessage();
		virtual ~BaseMessage() {};

		//prepare for departure and receive
		void prepare_send_buffer();
		void prepare_receive_buffer();
		
		//buffers
		inline auto header_buffer() { return boost::asio::buffer(m_header_buffer,HEADER_SIZE); }
		inline auto msg_buffer() { return boost::asio::buffer(m_msg_buffer); }

		//parsing
		bool parse_header();
		bool parse_msg();

	private: 
		std::string create_header(std::size_t);

	protected:
		json m_msg = {};

	private:
		std::vector<std::uint8_t> m_msg_buffer;
		std::vector<std::uint8_t> m_header_buffer;
		std::size_t m_msg_size;
	};

	class ProtocolMessage : public BaseMessage
	{

	public:
		enum class MsgType { REGISTER, MSG, NONE, UNKNOWN};

		void register_user(const UserData& user);
		void chat_message(const MessageData& msg);

		MsgType type();
		
		std::optional<std::string> msg();
		std::optional<UserData> user();


	};

}

