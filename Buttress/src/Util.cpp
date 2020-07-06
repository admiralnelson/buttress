#include "pch.h"
#include "Util.h"

std::string ReadFileAsString(std::string path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	if (t.is_open())
	{		
		buffer << t.rdbuf();
		return buffer.str();
	}
	PRINT("WARNING", "fail to load", path);
	return "";
}
std::vector<unsigned char> ReadFileAsByte(std::string path)
{
	std::ifstream f(path, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(f), {});
	if (f.is_open())
	{
		PRINT("WARNING", "fail to load", path);
	}	
	return buffer;
}