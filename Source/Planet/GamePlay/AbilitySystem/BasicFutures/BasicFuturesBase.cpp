
#include "BasicFuturesBase.h"

#include "AssetRefMap.h"
#include "CharacterBase.h"

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

// void UBasicFuturesBase::InitalDefaultTags()
// {
// 	Super::InitalDefaultTags();
// }

UGameplayEffect* UBasicFuturesBase::GetCooldownGameplayEffect() const
{
	return UAssetRefMap::GetInstance()->DurationGEClass.GetDefaultObject();
}

UGameplayEffect* UBasicFuturesBase::GetCostGameplayEffect() const
{
	return UAssetRefMap::GetInstance()->OnceGEClass.GetDefaultObject();
}
