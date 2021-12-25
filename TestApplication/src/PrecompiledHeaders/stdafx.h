#ifndef STDAFX_H
#define STDAFX_H

// Custom types:
typedef          bool      bool8;

typedef          char      char8;

typedef signed   char      int8;
typedef signed   short     int16;
typedef signed   int       int32;
typedef signed   long long int64;

typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

typedef float              float32;
typedef double             float64;

// Disable warnings from Vulkan SDK:
#pragma warning( push ) // Vulkan SDK - Begin
#pragma warning( disable : 26812 ) // Uncoped Enum

// GLFW and Vulkan:
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// ShaderC:
#pragma warning( disable : 26439 ) // Warning C26439: This kind of function may not throw.Declare it 'noexcept' (f.6).

// Warning LNK4099 is ignored!!! For more details look at: Properties->Linker->Command Line

#include <shaderc/shaderc.hpp>

#pragma warning( pop ) // Vulkan SDK - End

// C++ core:
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>
#include <map>
#include <unordered_map>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <functional>

#endif // STDAFX_H