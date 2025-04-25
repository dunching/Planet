#include "STE_CharacterBase.h"

inline void USTE_CharacterBase::TreeStart(
	FStateTreeExecutionContext& Context
)
{
	Super::TreeStart(Context);

	GloabVariable_Character = NewObject<UGloabVariable_Character>();
}

void USTE_CharacterBase::UpdateTargetCharacter(
	FStateTreeExecutionContext& Context
)
{
}
