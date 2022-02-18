
#include "../inc/myfunc_string.h"

#include <sstream>

namespace TKGEngine::MyFunc
{
	////////////////////////////////////////////////////////
	// Global Function definition
	////////////////////////////////////////////////////////
	std::pair<std::string, std::string> SplitFileName(const std::string& path, const char* splitter)
	{
		std::pair<std::string, std::string> sp_name;
		const size_t idx = path.find(splitter);
		if (idx == std::string::npos)
		{
			sp_name.first = path;
		}
		else
		{
			sp_name.first = path.substr(0, idx);
			sp_name.second = path.substr(idx + 1, path.size() - idx - 1);
		}
		return sp_name;
	}

	std::vector<std::string> SplitString(const std::string& str, char sep)
	{
		std::vector<std::string> ret_vec;

		std::stringstream ss(str);
		std::string out;

		while (std::getline(ss, out, sep))
		{
			ret_vec.emplace_back(out);
		}
		return ret_vec;
	}

	void ToLower(std::string& str)
	{
		std::transform(str.cbegin(), str.cend(), str.begin(), tolower);
	}

	void ToUpper(std::string& str)
	{
		std::transform(str.cbegin(), str.cend(), str.begin(), toupper);
	}

}	// namespace TKGEngine::MyFunc