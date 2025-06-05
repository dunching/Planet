
#include "BasicFutures_SwitchWeapon.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"
#include "ProxyProcessComponent.h"

inline void UBasicFutures_SwitchWeapon::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CharacterPtr)
	{
		if (CharacterPtr->GetProxyProcessComponent()->SwitchWeapon())
		{
#if UE_EDITOR || UE_SERVER
			if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
			{
				CommitAbility(Handle, ActorInfo, ActivationInfo);
			}
#endif
		}
	}
	
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		K2_CancelAbility();
	}
#endif
}

bool UBasicFutures_SwitchWeapon::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags
	) const
{
	auto InTags = FGameplayTagContainer::EmptyContainer;

	InTags.AddTag(UGameplayTagsLibrary::BaseFeature_SwitchWeapon);
	InTags.AddTag(UGameplayTagsLibrary::GEData_CD);

	const auto GameplayEffectHandleAry = GetAbilitySystemComponentFromActorInfo()->
																   GetActiveEffectsWithAllTags(
																	   InTags
																   );

	if (!GameplayEffectHandleAry.IsEmpty())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBasicFutures_SwitchWeapon::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::BaseFeature_SwitchWeapon);
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_CD);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			UGameplayTagsLibrary::GEData_Duration,
			CD
		);

		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void UBasicFutures_SwitchWeapon::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::BaseFeature_SwitchWeapon);
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			UGameplayTagsLibrary::GEData_ModifyItem_Mana,
			Cost
		);

		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}
