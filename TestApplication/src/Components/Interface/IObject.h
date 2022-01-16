#ifndef IOBJECT_H
#define IOBJECT_H

namespace t3d
{
	class IObject
	{
	public:

	// Constructors and Destructor:

		         IObject() = default;

		virtual ~IObject() {};

	// Functions:

		virtual void Start()  = 0;

		virtual void Update(const float32& DeltaTime) = 0;
	};
}

#endif // IROBJECT_H