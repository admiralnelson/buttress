#include "pch.h"
#include "Util.h"
#include "Logger.h"
static std::mutex mutexLogger;

void Print(std::stringstream& s)
{
	std::lock_guard<std::mutex> guard(mutexLogger);
	std::cout << " THREAD " << std::this_thread::get_id() << s.str() << std::endl;
}

int Random(const int min, const int max)
{
	static thread_local std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

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

std::string RandomString(int n)
{
	std::string out;
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (size_t i = 0; i < n; i++)
	{
		out += alphanum[Random(0, (sizeof(alphanum) - 1))];
	}
	return out;
}

long long GetSystemTime()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

double GetCurrentTimeInSecs()
{
	using namespace std::chrono;
	return (double)duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}
