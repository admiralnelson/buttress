// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define TBB_PREVIEW_CONCURRENT_ORDERED_CONTAINERS 1

// add headers that you want to pre-compile here
#include <memory>
#include <string>
#include <iostream>
#include <functional>
#include <algorithm>
#include <set>
#include <array>
#include <unordered_map>
#include <map>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <fstream>
#include <sstream>
#include <queue>
#include <deque>
#include <regex>
#include <mutex>
#include <shared_mutex>
#include <condition_variable> 
#include <future>
#include <thread>
#include <bitset>
#include <any>
#include <chrono>
#include <atomic>
#include <random>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <tbb/tbb.h>
#include "Geometry.h"
#include "Logger.h"


#if defined(WIN32) || defined(WIN64)
#   define WINDOWS
#elif defined(__unix__)
#   define LINUX
#endif

#ifdef WINDOWS
#   define NOMINMAX
#   define STRICT
#	define NOGDI
#	define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#   include "windows.h"
#endif

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "core/Types.h"
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#ifdef _DEBUG
//#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else


#endif


#endif //PCH_H
