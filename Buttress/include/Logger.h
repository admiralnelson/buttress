#pragma once
#include <string>
#include <iostream>
#include <utility>
#include <iomanip>

#define PRINT(...) std::cout << "[" << __FUNCTION__ << " " <<  __FILE__ << ":" << __LINE__ << "]  " , __VA_ARGS__ , std::endl

template <typename T>
inline std::ostream& operator,(std::ostream& out, const T& t) {
    out << " " << t;
    return out;
}

//overloaded version to handle all those special std::endl and others...
inline std::ostream& operator,(std::ostream& out, std::ostream& (*f)(std::ostream&)) {
    out  << f;
    return out;
}