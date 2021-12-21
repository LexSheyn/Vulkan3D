#ifndef TIMESTEP32_H
#define TIMESTEP32_H

// #include <chrono>

namespace t3d
{
	/// <summary>
	/// Advanced float32 that supposed to hold a time interval in seconds by default.
	/// </summary>
	class TimeStep32
	{
	public:

	// Constructor:

		TimeStep32( float32 time = 0.0f );

	// Operators:

		operator::float32() const { return m_Time; }

	// Accessors:

		inline float32 GetSeconds()      const { return m_Time; }

		inline float32 GetMilliseconds() const { return m_Time * 1000.0f; }

	private:

	// Variables:

		float32 m_Time;
	};
}

#endif // TIMESTEP32_H