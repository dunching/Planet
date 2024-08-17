
#include "Skill_Consumable_Base.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagsSubSystem.h"
#include "Skill_Consumable_Generic.h"
#include "PlanetAbilitySystemComponent.h"

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
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->UsingConsumable);

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Consumable*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			UnitPtr = GameplayAbilityTargetDataPtr->UnitPtr;
		}
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

bool USkill_Consumable_Base::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	UnitPtr->ApplyCooldown();

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_Consumable_Base::ContinueActive(UConsumableUnit* InUnitPtr)
{
	if (!CanActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
	{
		return;
	}

	UnitPtr = InUnitPtr;

	PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
}

void USkill_Consumable_Base::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	CommitAbility(Handle, ActorInfo, ActivationInfo);
}