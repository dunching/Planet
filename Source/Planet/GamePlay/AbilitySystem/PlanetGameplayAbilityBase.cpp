#include "PlanetGameplayAbilityBase.h"

#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"

UGameplayEffect* UPlanetGameplayAbilityBase::GetCooldownGameplayEffect() const
{
	return UAssetRefMap::GetInstance()->DurationGEClass.GetDefaultObject();
}

UGameplayEffect* UPlanetGameplayAbilityBase::GetCostGameplayEffect() const
{
	return UAssetRefMap::GetInstance()->OnceGEClass.GetDefaultObject();
}

bool UPlanetGameplayAbilityBase::CheckCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags
	) const
{
	if (!ensure(ActorInfo))
	{
		return true;
	}

	TArray<FGameplayTag> SourceTags{UGameplayTagsLibrary::GEData_CD};

	const FGameplayTagContainer CooldownTags = FGameplayTagContainer::CreateFromArray(SourceTags);

	if (!CooldownTags.IsEmpty())
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			if (AbilitySystemComponent->HasAnyMatchingGameplayTags(CooldownTags))
			{
				return false;
			}
		}
	}
	return true;
}

const FGameplayTagContainer* UPlanetGameplayAbilityBase::GetCooldownTags() const
{
	return nullptr;
}

void UPlanetGameplayAbilityBase::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	ApplyCostImp(Handle, ActorInfo, ActivationInfo, GetCostMap());
}

bool UPlanetGameplayAbilityBase::CheckCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags
	) const
{
	if (!ensure(ActorInfo))
	{
		return true;
	}

	auto AbilitySystemComponentPtr = Cast<UCharacterAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (!AbilitySystemComponentPtr)
	{
		return false;
	}

	const auto CostsMap = AbilitySystemComponentPtr->GetCost(GetCostMap());
	return AbilitySystemComponentPtr->CheckCost(CostsMap);
}

void UPlanetGameplayAbilityBase::ApplyCostImp(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const TMap<FGameplayTag, int32>& CostMap
	) const
{
	auto AbilitySystemComponentPtr = Cast<UCharacterAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (!AbilitySystemComponentPtr)
	{
		return;
	}

	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), GetAbilityLevel());

		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);

		const auto CostsMap = AbilitySystemComponentPtr->GetCost(CostMap);
		for (const auto& Iter : CostsMap)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               Iter.Key,
			                                               -Iter.Value
			                                              );
		}
		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

TArray<FActiveGameplayEffectHandle> UPlanetGameplayAbilityBase::MyApplyGameplayEffectSpecToTarget(
	const FGameplayAbilitySpecHandle AbilityHandle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayEffectSpecHandle SpecHandle,
	const FGameplayAbilityTargetDataHandle& TargetData
	) const
{
	return ApplyGameplayEffectSpecToTarget(AbilityHandle, ActorInfo, ActivationInfo, SpecHandle, TargetData);
}

TMap<FGameplayTag, int32> UPlanetGameplayAbilityBase::GetCostMap() const
{
	return {};
}
