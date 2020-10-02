#pragma once
#include "pch.h"
std::string ReadFileAsString(std::string path);
std::vector<unsigned char> ReadFileAsByte(std::string path);
std::string RandomString(int n);
long long GetCurrentTime();
double GetCurrentTimeInSecs();