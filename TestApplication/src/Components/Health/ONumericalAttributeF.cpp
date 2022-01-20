#include "../../PrecompiledHeaders/stdafx.h"
#include "ONumericalAttributeF.h"

namespace t3d
{
// Constructors and Destructor:

	ONumericalAttributeF::ONumericalAttributeF(float32 ValueMax, float32 Value)
		: ValueMax(ValueMax),
		  Value(Value)
	{
	}

	ONumericalAttributeF::~ONumericalAttributeF()
	{
	}


// Functions:

	void ONumericalAttributeF::Start()
	{
	}

	void ONumericalAttributeF::Update(const float32& DeltaTime)
	{
		if (Value > ValueMax)
		{
			Value = ValueMax;
		}
		else if (Value < 1.0f)
		{
			Value = 0.0f;
		}
	}

}