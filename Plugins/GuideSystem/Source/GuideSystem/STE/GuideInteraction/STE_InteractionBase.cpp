#include "STE_InteractionBase.h"

void USTE_InteractionBase::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable_InteractionBase>();
}

void USTE_InteractionBase::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
}
