
#include "BasicFuturesBase.h"

UBasicFuturesBase::UBasicFuturesBase() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBasicFuturesBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	InitialTags();
}

void UBasicFuturesBase::InitialTags()
{

}
