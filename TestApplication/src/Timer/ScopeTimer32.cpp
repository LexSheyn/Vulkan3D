#include "../PrecompiledHeaders/stdafx.h"
#include "ScopeTimer32.h"

namespace t3d
{
// Constructors and Destructor:

	ScopeTimer32::ScopeTimer32()
		: m_Duration( 0.0f )
	{
		m_Start = std::chrono::high_resolution_clock::now();
	}

	ScopeTimer32::~ScopeTimer32()
	{
		m_Finish = std::chrono::high_resolution_clock::now();

		m_Duration = m_Finish - m_Start;

		float32 ms = m_Duration.count() * 1000.0f;

		std::cout << "Scope timer took: " << ms << "ms" << std::endl;
	}

}