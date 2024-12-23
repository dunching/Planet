// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GenerateType.h"
#include "SceneElement.h"
#include "PlanetGameplayAbility.generated.h"

class UPlanetAbilitySystemComponent;

USTRUCT()
struct FGameplayAbilityTargetData_RegisterParam :
	public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual FGameplayAbilityTargetData_RegisterParam* Clone()const;

private:

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_RegisterParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_RegisterParam>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct FGameplayAbilityTargetData_ActiveParam :
	public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual FGameplayAbilityTargetData_ActiveParam* Clone()const;

	int32 ID = 0;

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ActiveParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ActiveParam>
{
	enum
	{
		WithNetSerializer = true,
	};
};

/**
 *
 */
UCLASS()
class PLANET_API UPlanetGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	friend UPlanetAbilitySystemComponent;

	UPlanetGameplayAbility();

	void SetContinuePerform(bool bIsContinue);

#if WITH_EDITOR
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

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	);

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility
	)override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

	virtual void OnGameplayTaskInitialized(UGameplayTask& Task) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
#endif

	// 通过此函数修改GAS上记录的CDO的Tags
	virtual	void InitalDefaultTags();

protected:

	virtual void SetContinuePerformImp(bool bIsContinue);

	void RunIfListLock()const;

	void ResetListLock()const;

	void DecrementToZeroListLock()const;

	void DecrementListLockOverride()const;

};
