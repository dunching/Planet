
#include "GuideSystemStateTreeComponent.h"

#include "StateTreeExecutionContext.h"

#include "GuideActor.h"

FName UGuideSystemStateTreeComponent::ComponentName = TEXT("GuideSystemStateTreeComponent");

UGuideSystemStateTreeComponent::UGuideSystemStateTreeComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	// 因为有切换任务 所以这里我们不要他自动调用，
	bStartLogicAutomatically = false;
}

UGameplayTasksComponent* UGuideSystemStateTreeComponent::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return GetOwner<AGuideActor>()->GetGameplayTasksComponent();
}

void UGuideSystemStateTreeComponent::StartLogic()
{
	Super::StartLogic();
}

void UGuideSystemStateTreeComponent::StopLogic(const FString& Reason)
{
	Super::StopLogic(Reason);
}
