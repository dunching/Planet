#include "AnimNotifyState_AddTagInDuration.h"

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
			CharacterOwnerPtr->GetCharacterAbilitySystemComponent()->AddLooseGameplayTag(
				 UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove
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
			CharacterOwnerPtr->GetCharacterAbilitySystemComponent()->RemoveLooseGameplayTag(
				 UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove
				);
		}
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
