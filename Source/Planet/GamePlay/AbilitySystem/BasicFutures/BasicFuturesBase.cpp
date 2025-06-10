
#include "BasicFuturesBase.h"

#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"

UBasicFuturesBase::UBasicFuturesBase() :
                                       Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UBasicFuturesBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
	}
}
