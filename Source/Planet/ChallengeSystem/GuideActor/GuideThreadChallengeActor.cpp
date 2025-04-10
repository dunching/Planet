#include "GuideThreadChallengeActor.h"

#include "GameplayTasksComponent.h"
#include "StateTreeConditionBase.h"
#include "STT_GuideThread.h"

#include "GuideSystemStateTreeComponent.h"

#include "STT_GuideThread_Challenge.h"

bool UStateTreeGuideThreadChallengeComponentSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return Super::IsStructAllowed(InScriptStruct) ||
		InScriptStruct->IsChildOf(FSTT_GuideThreadChallengeBase::StaticStruct()) ;
}

TSubclassOf<UStateTreeSchema> UGuideChallengeSystemStateTreeComponent::GetSchema() const
{
	return UStateTreeGuideThreadChallengeComponentSchema::StaticClass();
}

AGuideChallengeThread::AGuideChallengeThread(const FObjectInitializer& ObjectInitializer):
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UGuideChallengeSystemStateTreeComponent>(
			UGuideSystemStateTreeComponent::ComponentName
			)
	)
{
}
