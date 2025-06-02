// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "ItemProxy_Minimal.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Active_Base.h"

#include "Skill_Consumable_Base.generated.h"

class AConsumable_Base;
class ACharacterBase;

struct FConsumableProxy;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RegisterParam_Consumable : 
	public FGameplayAbilityTargetData_RegisterParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	FGuid ProxyID;

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_RegisterParam_Consumable> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_RegisterParam_Consumable>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class USkill_Consumable_Base : 
	public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:

	using FRegisterParamType = FGameplayAbilityTargetData_RegisterParam_Consumable;

	USkill_Consumable_Base();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) override;

	virtual bool CanUse()const;

	void ContinueActive(const TSharedPtr<FConsumableProxy>& ProxyPtr);

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	ACharacterBase* CharacterPtr = nullptr;

	TSharedPtr<FConsumableProxy> ProxyPtr = nullptr;

	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>CooldownMap;

};
