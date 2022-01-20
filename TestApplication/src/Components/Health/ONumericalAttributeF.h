#ifndef FNUMERICALATTRIBUTEF_H
#define FNUMERICALATTRIBUTEF_H

#include "../Interface/IObject.h"

namespace t3d
{
	class ONumericalAttributeF : public IObject
	{
	public:

	// Constructors and Destructor:

		         ONumericalAttributeF(float32 ValueMax, float32 Value);

		virtual ~ONumericalAttributeF();

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

#endif // FNUMERICALATTRIBUTEF_H