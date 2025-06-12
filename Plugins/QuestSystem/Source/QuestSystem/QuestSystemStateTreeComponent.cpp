
#include "QuestSystemStateTreeComponent.h"

#include "StateTreeExecutionContext.h"

#include "QuestsActorBase.h"

FName UQuestSystemStateTreeComponent::ComponentName = TEXT("GuideSystemStateTreeComponent");

UQuestSystemStateTreeComponent::UQuestSystemStateTreeComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	// 因为有切换任务 所以这里我们不要他自动调用，
	bStartLogicAutomatically = false;
}

UGameplayTasksComponent* UQuestSystemStateTreeComponent::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return GetOwner<AQuestActorBase>()->GetGameplayTasksComponent();
}

void UQuestSystemStateTreeComponent::StartLogic()
{
	Super::StartLogic();
}

void UQuestSystemStateTreeComponent::StopLogic(const FString& Reason)
{
	Super::StopLogic(Reason);
}
