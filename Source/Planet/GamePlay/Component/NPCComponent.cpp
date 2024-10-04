
#include "NPCComponent.h"

#include "GeneratorColony.h"
#include "CharacterBase.h"
#include "HumanAIController.h"

void UNPCComponent::BeginPlay()
{
	Super::BeginPlay();

	auto OwnerPtr = GetOwner();
	if (!OwnerPtr)
	{
		return;
	}

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// 这里按次序获取要追踪的位置
// 		TArray<AActor*> OutActors;
// 		OwnerPtr->GetAttachedActors(OutActors);
// 
// 		auto ParentPtr = OwnerPtr->GetAttachParentActor();
// 		if (auto ColonyPtr = Cast<AGeneratorColony>(ParentPtr))
// 		{
// 			const auto ChildAry = ColonyPtr->FormationComponentPtr->GetAttachChildren();
// 			if (ChildAry.Num() >= OutActors.Num())
// 			{
// 				auto OwnerCharacterPtr = Cast<ACharacterBase>(OwnerPtr);
// 				if (OwnerCharacterPtr)
// 				{
// 					OwnerCharacterPtr->GetController<AHumanAIController>()->PathFollowComponentPtr = ChildAry[OutActors.Num() - 1];
// 				}
// 			}
// 		}
	}
#endif

	OwnerPtr->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}
