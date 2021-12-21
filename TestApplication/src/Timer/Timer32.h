#ifndef TIMER32_H
#define TIMER32_H

// #include <chrono>

namespace t3d
{
	class Timer32
	{
	public:

	// Constructor:

		Timer32();

	// Functions:

		void Restart();

		float32 Mark();

		float32 Peek() const;

	private:

	// Variables:

		std::chrono::steady_clock::time_point m_LastTimePoint;

		std::chrono::steady_clock::time_point m_PreviousTimePoint;

		std::chrono::duration<float32> m_Duration;
	};
}

#endif // TIMER32_H