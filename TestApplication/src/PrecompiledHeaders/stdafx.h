#ifndef STDAFX_H
#define STDAFX_H

// Disable warnings from Vulkan SDK:
#pragma warning( push ) // Vulkan SDK - Begin
#pragma warning( disable : 26812) // Uncoped Enum

// GLFW and Vulkan:
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#pragma warning( pop ) // Vulkan SDK - End

// C++ core:
#include <iostream>
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

#endif // STDAFX_H