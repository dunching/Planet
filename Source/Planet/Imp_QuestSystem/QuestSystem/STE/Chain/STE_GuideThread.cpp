#include "STE_GuideThread.h"

#include "Kismet/GameplayStatics.h"

#include "QuestsActorBase.h"
#include "QuestSubSystem.h"
#include "QuestChain.h"
#include "HumanCharacter_Player.h"

void USTE_GuideThread_Area::TreeStart(
	FStateTreeExecutionContext& Context
	)
{
	Super::TreeStart(Context);
	
	GloabVariable_AreaPtr = NewObject<UGloabVariable_GuideThread_Area>();
}

void USTE_GuideThread_Area::TreeStop(
	FStateTreeExecutionContext& Context
	)
{
	Super::TreeStop(Context);
}
