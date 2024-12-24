
#include "Skill_Consumable_Base.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "Skill_Consumable_Generic.h"
#include "PlanetAbilitySystemComponent.h"
#include "CharacterBase.h"
#include "HoldingItemsComponent.h"

UScriptStruct* FGameplayAbilityTargetData_Consumable::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_Consumable::StaticStruct();
}

bool FGameplayAbilityTargetData_Consumable::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		Ar << ProxyID;
	}
	else if (Ar.IsLoading())
	{
		Ar << ProxyID;
	}

	return true;
}

USkill_Consumable_Base::USkill_Consumable_Base():
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = false;
}

void USkill_Consumable_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

	if (Spec.GameplayEventData.IsValid() && Spec.GameplayEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FRegisterParamType*>(Spec.GameplayEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			ProxyPtr = DynamicCastSharedPtr<FConsumableProxy>(
				CharacterPtr->GetHoldingItemsComponent()->FindProxy(GameplayAbilityTargetDataPtr->ProxyID)
			);
		}
	}
}

bool USkill_Consumable_Base::CanUse() const
{
	return false;
}

void USkill_Consumable_Base::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::UsingConsumable);

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
	}
}

void USkill_Consumable_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool USkill_Consumable_Base::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (ProxyPtr && ProxyPtr->CheckCooldown())
	{
		return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	}

	return false;
}

bool USkill_Consumable_Base::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	// ProxyPtr->ApplyCooldown();

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_Consumable_Base::ContinueActive(const TSharedPtr<FConsumableProxy>& InProxyPtr)
{
	if (!CanActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
	{
		return;
	}

 	ProxyPtr = InProxyPtr;
 
 	PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
}

void USkill_Consumable_Base::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
}