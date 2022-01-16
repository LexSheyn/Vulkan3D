#ifndef FNUMERICALATTRIBUTE_H
#define FNUMERICALATTRIBUTE_H

#include "../Interface/IObject.h"

namespace t3d
{
	class ONumericalAttribute : public IObject
	{
	public:

	// Constructors and Destructor:

		ONumericalAttribute();
		ONumericalAttribute(float32 ValueMax, float32 Value);

		virtual ~ONumericalAttribute();

	// Functions:

		virtual void Start()                          override;

		virtual void Update(const float32& DeltaTime) override;

	// Accessors:

		inline float32& GetValueMax() { return ValueMax; }

		inline float32& GetValue()    { return Value; }

	private:

	// Variables:

		float32 ValueMax;
		float32 Value;
	};	
}

#endif // FNUMERICALATTRIBUTE_H