// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "ItemProxy_Minimal.h"
#include "Skill_Active_Base.h"

#include "Skill_Consumable_Base.generated.h"

class AConsumable_Base;
class ACharacterBase;

USTRUCT()
struct FGameplayAbilityTargetData_Consumable : 
	public FGameplayAbilityTargetData_RegisterParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	FGuid ProxyID;

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Consumable> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Consumable>
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

	using FRegisterParamType = FGameplayAbilityTargetData_Consumable;

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
	);

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
	);

	virtual bool CanUse()const;

	void ContinueActive(const TSharedPtr<FConsumableProxy>& UnitPtr);

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	ACharacterBase* CharacterPtr = nullptr;

	TSharedPtr<FConsumableProxy> UnitPtr = nullptr;

	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>CooldownMap;

};
