#pragma once
#include "pch.h"

#define SET_BIT_NTH(x, n) (x = (x | (1 << n)))
#define CHECK_BIT_NTH(x, n) ((x & (1 << n)) != 0)
#define UNSET_BIT_NTH(x, n) (x = (x & ~(1 << n)))
#define IS_ALL_BITS_SET(x) ((((x + 1) & x) == 0) && (x != 0))

int Random(const int min, const int max);
std::string ReadFileAsString(std::string path);
std::vector<unsigned char> ReadFileAsByte(std::string path);
std::string RandomString(int n);
long long GetSystemTime();
double GetCurrentTimeInSecs();