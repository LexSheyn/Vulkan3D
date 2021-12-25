#include "../PrecompiledHeaders/stdafx.h"
#include "Logger.h"

namespace t3d
{
// Static Variables:

	Logger::Level Logger::m_LogLevel = Level::Trace;

// Constructors and Destructor:

	Logger::Logger()
	{
	}

	Logger::~Logger()
	{
	}


// Functions:

	void Logger::Trace(const char8* functionName, const char8* message)
	{
		if ( m_LogLevel == Level::Trace )
		{
			std::cout << "[  Trace  ]::" << functionName << ": " << message << std::endl;
		}
	}

	void Logger::Warning(const char8* functionName, const char8* message)
	{
		if ( m_LogLevel <= Level::Warning )
		{
			std::cout << "[ Warning ]::" << functionName << ": " << message << std::endl;
		}
	}

	void Logger::Error(const char8* functionName, const char8* message)
	{
		if ( m_LogLevel <= Level::Error )
		{
			std::cout << "[  Error  ]::" << functionName << ": " << message << std::endl;
		}
	}


// Modifiers:

	void Logger::SetLevel(const Level logLevel)
	{
		m_LogLevel = logLevel;
	}

}