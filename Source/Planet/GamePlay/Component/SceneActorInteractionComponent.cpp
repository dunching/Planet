#include "SceneActorInteractionComponent.h"

#include "GuideActor.h"

FName USceneActorInteractionComponent::ComponentName = TEXT("SceneActorInteractionComponent");

USceneActorInteractionComponent::USceneActorInteractionComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	// 因为有切换任务 所以这里我们不要他自动调用，
	// bStartLogicAutomatically = false;
}

TArray<TSubclassOf<AGuideInteractionActor>>  USceneActorInteractionComponent::GetTaskNodes() const
{
	TArray<TSubclassOf<AGuideInteractionActor>> Results;
	
	Results.Append(GuideInteractionAry);
	Results.Append(TemporaryGuideInteractionAry);
	
	return Results;
}

void USceneActorInteractionComponent::AddGuideActor(const TSubclassOf<AGuideInteractionActor>& GuideActorClass)
{
	TemporaryGuideInteractionAry.Add(GuideActorClass);
}

 void USceneActorInteractionComponent::RemoveGuideActor(const TSubclassOf<AGuideInteractionActor>& GuideActorClass)
{
	for (int32 Index = 0; Index < TemporaryGuideInteractionAry.Num(); Index++)
	{
		if (TemporaryGuideInteractionAry[Index] == GuideActorClass)
		{
			TemporaryGuideInteractionAry.RemoveAt(Index);
			break;
		}
	}
}