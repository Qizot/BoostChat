#pragma once

namespace chat
{

	const std::string currentDateTime();
	const std::string currentTime();

	struct BaseConverter
	{
		static int HexToDec(std::string);
		static std::string DecToHex(std::size_t);
	};

	void replace_inplace(std::string& in, const std::string& from, const std::string& to);
	

}

