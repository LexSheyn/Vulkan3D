#ifndef SCOPETIMER32_H
#define SCOPETIMER32_H

// #include <chrono>

// Time Units:
//
// milliseconds 10^-3  = 1 / 1.000 s
// microsecends 10^-6  = 1 / 1.000.000 s
// nanoseconds  10^-9  = 1 / 1.000.000.000 s
// picoseconds  10^-12 = 1 / 1.000.000.000.000 s
// femtoseconds 10^-15 = 1 / 1.000.000.000.000.000 s

namespace t3d
{
	/// <summary>
	/// Mesures scope compute time duration from begin to end in milliseconds.
	/// </summary>
	class ScopeTimer32
	{
	public:

	// Constructors adn Destructor:

		ScopeTimer32();

		~ScopeTimer32();

	private:

	// Variables:

		std::chrono::time_point<std::chrono::steady_clock> m_Start;

		std::chrono::time_point<std::chrono::steady_clock> m_Finish;

		std::chrono::duration<float32> m_Duration;
	};
}

#endif // SCOPETIMER32_H