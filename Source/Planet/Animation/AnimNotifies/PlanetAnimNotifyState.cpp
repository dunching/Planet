#include "PlanetAnimNotifyState.h"

#include "CharacterAbilitySystemComponent.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"

auto UAnimNotifyState_AddTagInDuration::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
	) -> void
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UWorld* World = MeshComp->GetWorld();
	if (World && (World->IsGameWorld()))
	{
		auto CharacterOwnerPtr = MeshComp->GetOwner<FOwnerPawnType>();
		if (CharacterOwnerPtr)
		{
			CharacterOwnerPtr->GetCharacterAbilitySystemComponent()->AddLooseGameplayTags(
				 ActivedTags
				);
		}
	}
}

void UAnimNotifyState_AddTagInDuration::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
	)
{
	UWorld* World = MeshComp->GetWorld();
	if (World && (World->IsGameWorld()))
	{
		auto CharacterOwnerPtr = MeshComp->GetOwner<FOwnerPawnType>();
		if (CharacterOwnerPtr)
		{
			CharacterOwnerPtr->GetCharacterAbilitySystemComponent()->RemoveLooseGameplayTags(
				 ActivedTags
				);
		}
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
