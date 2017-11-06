#include <string>
#include <cctype>
#include <sstream>

unsigned countWords(const std::string &str)
{
	if (str == "")
		return 0;
	bool in_word = !isspace(str[0]);
	unsigned result = in_word?1:0;
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!in_word && !isspace(str[i]))
		{
			in_word = true;
			result++;
		}
		if (isspace(str[i]))
			in_word = false;
	}
	return result;
}

std::string getDirectory(const std::string &path)
{
	const size_t last_slash_idx = path.rfind('/');
	if (std::string::npos != last_slash_idx)
		return path.substr(0, last_slash_idx);
	else
		return std::string("./");
}

std::string getFilename(const std::string &path)
{
	const size_t last_slash_idx = path.rfind('/');
	if (std::string::npos != last_slash_idx)
		return path.substr(last_slash_idx + 1, std::string::npos);
	else
		return path;
}

std::string getFirtsWord(const std::string &str)
{
	if (countWords(str) == 0)
		return std::string("");
	std::istringstream words(str);
	std::string result;
	words>>result;
	return result;
}