#include "../../PrecompiledHeaders/stdafx.h"
#include "ORegen.h"

namespace t3d
{
// Constructors and Destructor:

	ORegen::ORegen(float32 PointsPerSecond, float32 Duration)
		: PointsPerSecond(PointsPerSecond), Duration(Duration)
	{
	}

	ORegen::~ORegen()
	{
	}


// Functions:

	void ORegen::Start()
	{
	}

	void ORegen::Update(const float32& DeltaTime)
	{
		Duration -= DeltaTime;
	}

}