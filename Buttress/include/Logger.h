#pragma once
#include <string>
#include <iostream>
#include <utility>
#include <iomanip>


#define PRINT(...) { std::stringstream ss; ss , "[" , __FUNCTION__ , " " , __FILE__ ,":" , __LINE__ , "] ", __VA_ARGS__; Print(ss); }
void Print(std::stringstream& s);

template <typename T>
inline std::stringstream& operator,(std::stringstream& out, const T& t) {
	//std::lock_guard<std::mutex> guard(mutexLogger);
	out << " " << t;
	return out;
}

inline std::stringstream& operator,(std::stringstream& out, std::stringstream& (*f)(std::stringstream&)) {
	//std::lock_guard<std::mutex> guard(mutexLogger);
	out << f;
	return out;
}