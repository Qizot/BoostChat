#pragma once
#include <cstdint>
#include <string>
#include <cstdlib>
#include <sstream>
#include <limits>
#include <iomanip>

/*
	ChatMessage consists of:
	- 4 bytes of hex value consisting number of message chars(leading byte is hex number of nickname chars)
	- accordingly to previous lenght, the nick name
	- message body


*/

template<typename T>
class ChatMessage
{
	using String = T;
	using length = size_t;
	using const_ptr = typename T::const_pointer;
	using ptr = typename T::pointer;
public:

	static constexpr size_t header_length = 4;

	ChatMessage()  
	{
		m_msg.reserve(header_length);
	};
	virtual ~ChatMessage() {};

public:
	length BodySize() const
	{
		return m_body_size;
	}
	length NickSize() const
	{
		return m_nick_size;
	}
	length MessageSize() const
	{
		return m_msg.size();
	}

	bool DecodeHeader() //decodes header and reserves space for string
	{
		
		ptr header = m_msg.data();
		ptr header_end = m_msg.data() + 3;
		ptr nick = header_end + 1;
		ptr nick_end = nick + 1;

		if constexpr(std::is_same<T::value_type, wchar_t>::value)
		{
			m_msg_size      = std::wstrtoull(header, &header_end, 16);
			m_nick_size = std::wstrtoull(nick, &nick_end, 16);
		}
		else if constexpr(std::is_same<T::value_type, char>::value)
		{
			m_msg_size = std::strtoull(header, &header_end, 16);
			m_nick_size = std::strtoull(nick, &nick_end, 16);
		}
		else
			return false;

		if (m_nick_size > std::numeric_limits<std::uint8_t>::max() || m_msg_size > std::numeric_limits<std::uint16_t>::max() || m_msg_size < m_nick_size)
			return false;

		m_msg.reserve(m_msg_size + header_length);

		return true; 
	}

	
	const_ptr NickData() const
	{
		return m_msg.data() + 5;
	}
	const_ptr BodyData() const
	{
		return m_msg.data() + 5 + NickSize();
	}
	const_ptr MessageData() const
	{
		return m_msg.data() + 5;
	}

	template<typename U>
	std::enable_if_t<std::is_same_v<T,U>,bool> CreateMessage(U&& name, U&& body) // used to create the message from nick and body strings
	{
		if (name.size() > std::numeric_limits<std::uint8_t>::max() || body.size() > std::numeric_limits<std::uint16_t>::max())
			return false;
		m_msg_size = name.size() + body.size() + 1; // additional 1 is for byte containing hex number of nickname characters
		m_nick_size = name.size();
		
		std::basic_stringstream<T::value_type> ss;
		ss << std::setfill('0') << std::setw(4) << std::hex << m_msg_size;
		ss << std::hex << m_nick_size << std::forward<T>(name) << std::forward<T>(body);
		m_msg = ss.str();
		
		return true;
	}
	void SetHeader(T header)
	{
		m_msg = header;
	}

	
	T GetMessageString() const
	{
		return m_msg;
	}
	T GetNickString() const
	{
		return m_msg.substr(5, m_nick_size);
	}
	T GetBodyString() const
	{
		return m_msg.substr(5 + m_nick_size, m_msg_size - m_nick_size);
	}
	

private:
	

private:
	length m_msg_size;
	length m_nick_size;
	String m_msg;
};

