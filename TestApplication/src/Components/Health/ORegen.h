#ifndef OREGEN_H
#define OREGEN_H

#include "../Interface/IObject.h"

namespace t3d
{
	class ORegen : public IObject
	{
	public:

	// Constructors and Destructor:

		 ORegen(float32 PointsPerSecond, float32 Duration);

		~ORegen();

	// Functions:

		void Start()                          override;

		void Update(const float32& DeltaTime) override;

	// Accessors:

		inline const float32& GetPointsPerSecond() const { return PointsPerSecond; }

		inline const float32& GetDuration()        const { return Duration; }		

	private:

	// Variables:

		float32 PointsPerSecond;

		float32 Duration;		
	};
}

#endif // OREGEN_H