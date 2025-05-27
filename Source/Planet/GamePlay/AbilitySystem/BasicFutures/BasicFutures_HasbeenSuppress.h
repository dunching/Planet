// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateType.h"
#include "BaseData.h"

#include "BasicFutures_HasbeenSuppress.generated.h"

class ATractionPoint;
class UAbilityTask_TimerHelper;
class UAbilityTask_HasBeenFlyAway;
class UGE_Running;
class ACharacterBase;
class UGE_CancelRunning;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress : public FGameplayAbilityTargetData_ActiveParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress* Clone() const override;

	TWeakObjectPtr<ACharacterBase> InstigatorPtr = nullptr;

	TSoftObjectPtr<UAnimMontage> MontageRef = nullptr;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress>
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
class PLANET_API UBasicFutures_HasbeenSuppress : public UBasicFuturesBase
{
	GENERATED_BODY()

	using FActiveParamType = FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress;

public:
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

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
		) override;

protected:
	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
		) override;

private:
	bool GetOrient(
		FRotator& DesiredRotation,
		bool& bIsImmediatelyRot
		);

	TSharedPtr<FActiveParamType> ActiveParamSPtr = nullptr;

	ACharacterBase* CharacterPtr = nullptr;

	FActiveGameplayEffectHandle SuppressGEHandle;
		
	const int32 GetOrientPrority = 1;
};
