#include "../../PrecompiledHeaders/stdafx.h"
#include "ONumericalAttribute.h"

namespace t3d
{
// Constructors and Destructor:

	ONumericalAttribute::ONumericalAttribute()
		: ValueMax(100.f), Value(ValueMax)
	{
	}

	ONumericalAttribute::ONumericalAttribute(float32 ValueMax, float32 Value)
		: ValueMax(ValueMax), Value(Value)
	{
	}

	ONumericalAttribute::~ONumericalAttribute()
	{
	}


// Functions:

	void ONumericalAttribute::Start()
	{
	}

	void ONumericalAttribute::Update(const float32& DeltaTime)
	{
		if (Value > ValueMax)
		{
			Value = ValueMax;
		}
		else if (Value < 0.0f)
		{
			Value = 0.0f;
		}
	}

}