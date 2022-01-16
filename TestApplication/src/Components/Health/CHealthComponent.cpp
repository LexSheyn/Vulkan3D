#include "../../PrecompiledHeaders/stdafx.h"
#include "CHealthComponent.h"

namespace t3d
{
// Constructors and Destructor:

	CHealthComponent::CHealthComponent(float32 HpMax, float32 Hp)
		: Health(HpMax, Hp)
	{

	}

	CHealthComponent::~CHealthComponent()
	{

	}


// Functions:

	void CHealthComponent::Start()
	{
	}

	void CHealthComponent::Update(const float32& DeltaTime)
	{
		uint32 Index = 0u;

		for (auto& Regen : RegenStacks)
		{
			Health.GetValue() += RegenStacks[Index]->GetPointsPerSecond() * DeltaTime;
			
			RegenStacks[Index]->Update(DeltaTime);

			if (RegenStacks[Index]->GetDuration() < 0.0f )
			{
				delete RegenStacks[Index];

				RegenStacks.erase(RegenStacks.begin() + Index);

				Index--;
			}

			Index++;
		}

		Health.Update(DeltaTime);
	}

	void CHealthComponent::Heal(float32 HpPerSecond, float32 Duration)
	{
		RegenStacks.push_back(new ORegen(HpPerSecond, Duration));
	}

	void CHealthComponent::Damage(float32 HpPerSecond, float32 Duration)
	{
		RegenStacks.push_back(new ORegen(-HpPerSecond, Duration));
	}

}