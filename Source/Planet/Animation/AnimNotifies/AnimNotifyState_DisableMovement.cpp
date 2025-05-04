#include "AnimNotifyState_DisableMovement.h"

#include "CharacterAbilitySystemComponent.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"

auto UAnimNotifyState_DisableMovement::NotifyBegin(
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

void UAnimNotifyState_DisableMovement::NotifyEnd(
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
