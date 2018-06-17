#include "stdafx.h"
#include "Utils.h"

namespace chat
{

	const std::string currentDateTime() 
	{
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
		return buf;
	}
	const std::string currentTime()
	{
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%T", &tstruct);
		return buf;
	}

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

	void replace_inplace(std::string& in, const std::string& from, const std::string& to)
	{
		auto it = in.find(from);
		while (it != -1)
		{
			in = in.replace(it, from.size(), to);
			it = in.find(from);
		}
	}

}