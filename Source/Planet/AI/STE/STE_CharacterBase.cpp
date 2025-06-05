#include "STE_CharacterBase.h"

#include "HumanCharacter_AI.h"

inline void USTE_CharacterBase::TreeStart(
	FStateTreeExecutionContext& Context
	)
{
	Super::TreeStart(Context);

	GloabVariable_Character = CreateGloabVarianble();
}

UGloabVariable_Character* USTE_CharacterBase::CreateGloabVarianble()
{
	return NewObject<UGloabVariable_Character>();
}

TWeakObjectPtr<ACharacterBase> USTE_CharacterBase::GetNewTargetCharacter(
	const TSet<TWeakObjectPtr<ACharacterBase>>& ChractersSet
	) const
{
	const auto Location = HumanCharacterPtr->GetActorLocation();

	TWeakObjectPtr<ACharacterBase> TargetPtr = nullptr;
	int32 Distance = 0;
	for (const auto Iter : ChractersSet)
	{
		if (Iter.IsValid())
		{
			auto NewDistance = FVector::Dist2D(Location, Iter->GetActorLocation());
			if (TargetPtr.IsValid())
			{
				if (NewDistance < Distance)
				{
					TargetPtr = Iter;
					Distance = NewDistance;
				}
			}
			else
			{
				TargetPtr = Iter;
				Distance = NewDistance;
			}
		}
	}

	return TargetPtr;
}

// void UGloabVariable_Character::UpdateTargetCharacter(
// 	FStateTreeExecutionContext& Context
// )
// {
// }
