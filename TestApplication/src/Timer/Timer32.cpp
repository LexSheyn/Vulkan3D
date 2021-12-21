#include "../PrecompiledHeaders/stdafx.h"
#include "Timer32.h"

namespace t3d
{
// Constructor:

	Timer32::Timer32()
		: m_Duration( 0.0f )
	{
		m_LastTimePoint = std::chrono::steady_clock::now();
	}


// Functions:

	void Timer32::Restart()
	{
		m_LastTimePoint = std::chrono::steady_clock::now();
	}

	float32 Timer32::Mark()
	{
		m_PreviousTimePoint = m_LastTimePoint;

		m_LastTimePoint = std::chrono::steady_clock::now();

		m_Duration = m_LastTimePoint - m_PreviousTimePoint;

		return m_Duration.count();
	}

	float32 Timer32::Peek() const
	{
		return std::chrono::duration<float32>( std::chrono::steady_clock::now() - m_LastTimePoint ).count();
	}

}