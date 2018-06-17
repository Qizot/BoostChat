#include "stdafx.h"
#include "ChatMessage.h"
#include <iomanip>


namespace chat {



	BaseMessage::BaseMessage() : m_header_buffer(HEADER_SIZE)
	{
		//m_header_buffer.resize(HEADER_SIZE);
		assert(m_header_buffer.size() == HEADER_SIZE);
	}


	void BaseMessage::prepare_send_buffer()
	{
		
		auto vec = json::to_cbor(m_msg);
		auto header = create_header(vec.size());
	
		std::copy(begin(header), end(header), begin(m_header_buffer));
		m_msg_buffer.resize(vec.size() + HEADER_SIZE);

		std::copy(begin(header), end(header), begin(m_msg_buffer));
		std::copy(begin(vec), end(vec), begin(m_msg_buffer) + HEADER_SIZE);

		m_msg_size = m_msg_buffer.size() + HEADER_SIZE;
	}

	void BaseMessage::prepare_receive_buffer()
	{
		m_msg_buffer.clear();
		m_msg_buffer.resize(m_msg_size);
	}

	bool BaseMessage::parse_header()
	{
		std::string n(begin(m_header_buffer), end(m_header_buffer));
		m_msg_size = BaseConverter::HexToDec(n);
		return m_msg_size > 0 ? true : false;
	}

	bool BaseMessage::parse_msg()
	{
		m_msg.clear();
		try
		{
			m_msg = json::from_cbor(m_msg_buffer);
		}
		catch (json::exception&)
		{
			return false;
		}
		return true;
	}
	std::string BaseMessage::create_header(std::size_t n)
	{
		std::ostringstream ss;
		ss << std::setfill('0') << std::setw(HEADER_SIZE) << BaseConverter::DecToHex(n);
		return ss.str();
	}
//====================================================================================
	void ProtocolMessage::register_user(const UserData & user)
	{
		auto[name, date] = user;
		m_msg.clear();
		m_msg["type"] = std::string("REGISTER");
		m_msg["name"] = name;
		m_msg["date"] = date;
	}
	void ProtocolMessage::chat_message(const MessageData & msg)
	{
		auto[name, content, time] = msg;
		m_msg.clear();
		m_msg["type"] = std::string("MSG");
		m_msg["name"] = name;
		m_msg["content"] = content;
		m_msg["time"] = time;
	}

	ProtocolMessage::MsgType ProtocolMessage::type()
	{
		auto t = m_msg.find("type");

		if (t == m_msg.end())
			return MsgType::NONE;
		else if (*t == "REGISTER")
			return MsgType::REGISTER;
		else if (*t == "MSG")
			return MsgType::MSG;
		else
			return MsgType::UNKNOWN;
	}
	std::optional<std::string> ProtocolMessage::msg()
	{
		try {
			if (type() == MsgType::MSG)
			{
				std::ostringstream ss;
				ss << m_msg["time"].get<std::string>() << ": " << m_msg["name"].get<std::string>() << ": " << m_msg["content"].get<std::string>();
				return ss.str();
			}
		}
		catch (std::exception& e) { std::cout << e.what() << "\n"; }

		return std::nullopt;
	}
	std::optional<UserData> ProtocolMessage::user()
	{
		try {
			if (type() == MsgType::REGISTER)
			{
				auto user = UserData({ m_msg["name"].get<std::string>(), m_msg["date"].get<std::string>() });
				return user;
			}
		}
		catch (std::exception& e) { std::cout << e.what() << "\n"; }

		return std::nullopt;
	}
}

