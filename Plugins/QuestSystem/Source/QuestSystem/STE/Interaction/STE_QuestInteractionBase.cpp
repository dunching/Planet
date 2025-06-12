#include "STE_QuestInteractionBase.h"

void USTE_QuestInteractionBase::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable_InteractionBase>();
}

void USTE_QuestInteractionBase::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
}
