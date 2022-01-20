#ifndef CHEALTHCOMPONENT_H
#define CHEALTHCOMPONENT_H

#include "../Interface/IComponent.h"

#include "ONumericalAttributeF.h"
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

		void Heal(float32 HpPerSecond, float32 Duration = 1.0f);

		void Damage(float32 HpPerSecond, float32 Duration = 1.0f);

	// Accessors:

		inline ONumericalAttributeF& GetHp() { return Health; }

	private:

	// Variables:

		ONumericalAttributeF Health;

		std::vector<ORegen*> RegenStacks;
	};
}

#endif // CHEALTHCOMPONENT_H