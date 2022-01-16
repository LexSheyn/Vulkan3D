#ifndef ICOMPONENT_H
#define ICOMPONENT_H

namespace t3d
{
	class IComponent
	{
	public:

	// Constructors and Destructor:

		         IComponent() = default;

		virtual ~IComponent() {};

	// Functions:

		virtual void Start()  = 0;

		virtual void Update(const float32& DeltaTime) = 0;
	};
}

#endif // ICOMPONENT_H