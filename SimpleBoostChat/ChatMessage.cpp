#include "stdafx.h"
#include "ChatMessage.h"
#include <boost\asio.hpp>


namespace chat {

	int BaseConverter::HexToDec(std::string a)
	{
		int n = std::strtol(a.c_str(), nullptr, 16);
		return n;
	}

	std::string BaseConverter::DecToHex(std::size_t n)
	{
		std::ostringstream ss;
		ss << std::hex << n;
		return ss.str();
	}

	BaseMessage::BaseMessage() : m_header_buffer(HEADER_SIZE) {}

	void BaseMessage::set_msg(std::string nickname, std::string body)
	{
		m_msg.clear();
		m_msg["nickname"] = nickname;
		m_msg["content"] = body;
	}

	std::optional<std::string> BaseMessage::get_msg()
	{
		try
		{
			std::ostringstream ss;
			ss << m_msg.at("nickname").get<std::string>() << ": " << m_msg.at("content").get<std::string>();
			return ss.str();
		}
		catch (json::exception&)
		{
			return std::nullopt;
		}

	}

	void BaseMessage::prepare_send_buffer()
	{
		m_msg_buffer.clear();
		auto vec = json::to_cbor(m_msg);
		auto header = BaseConverter::DecToHex(vec.size());
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
}

