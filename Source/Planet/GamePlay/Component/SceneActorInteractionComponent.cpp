#include "SceneActorInteractionComponent.h"

FName USceneActorInteractionComponent::ComponentName = TEXT("SceneActorInteractionComponent");

USceneActorInteractionComponent::USceneActorInteractionComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	// 因为有切换任务 所以这里我们不要他自动调用，
	// bStartLogicAutomatically = false;
}

TArray<TSubclassOf<AGuideInteractionActor>>  USceneActorInteractionComponent::GetTaskNodes() const
{
	return GuideInteractionAry;
}
