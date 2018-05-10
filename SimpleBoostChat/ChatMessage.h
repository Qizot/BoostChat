#pragma once
#include <nlohmann/json.hpp>
#include <optional>
#include <boost\asio.hpp>

namespace chat {
	class BaseMessage;
	using ChatMessage = BaseMessage;
	using json = nlohmann::json;

	struct BaseConverter
	{
		static int HexToDec(std::string);
		static std::string DecToHex(std::size_t);
	private:

	};

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

		//getter and setter for msg
		void set_msg(std::string nickname, std::string body);
		virtual std::optional<std::string> get_msg();

		//prepare for departure and receive
		void prepare_send_buffer();
		void prepare_receive_buffer();
		std::size_t msg_binary_size() const { return m_msg_buffer.size(); }
		
		//buffers
		auto header_buffer()
		{
			return boost::asio::buffer(m_header_buffer,HEADER_SIZE);
		}

		auto msg_buffer()
		{
			return boost::asio::buffer(m_msg_buffer);
		}

		//parsing
		bool parse_header();
		bool parse_msg();


	private:
		std::string create_header(std::size_t);
		std::size_t m_msg_size;
		json m_msg;
		std::vector<std::uint8_t> m_msg_buffer;
		std::vector<std::uint8_t> m_header_buffer;
		



	};

}

