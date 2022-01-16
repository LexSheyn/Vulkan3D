#ifndef CHEALTHCOMPONENT_H
#define CHEALTHCOMPONENT_H

#include "../Interface/IComponent.h"

#include "ONumericalAttribute.h"
#include "ORegen.h"

namespace t3d
{
	class CHealthComponent : public IComponent
	{
	public:

	// Constructors and Destructor:

		 CHealthComponent(float32 HpMax, float32 Hp);

		~CHealthComponent();

	// Functions:

		void Start()                          override;

		void Update(const float32& DeltaTime) override;

		void Heal(float32 HpPerSecond, float32 Duration = 0.5f);

		void Damage(float32 HpPerSecond, float32 Duration = 0.5f);

	private:

	// Variables:

		ONumericalAttribute Health;

		std::vector<ORegen*> RegenStacks;
	};
}

#endif // CHEALTHCOMPONENT_H