#pragma once
#include "pch.h"

#define PRINT(...) std::cout, __VA_ARGS__, std::endl

template <typename T>
std::ostream& operator,(std::ostream& out, const T& t) {
    out << t;
    return out;
}

//overloaded version to handle all those special std::endl and others...
std::ostream& operator,(std::ostream& out, std::ostream& (*f)(std::ostream&)) {
    out << "[" << __FUNCTION__ << ":" << __LINE__ << "]" << f;
    return out;
}