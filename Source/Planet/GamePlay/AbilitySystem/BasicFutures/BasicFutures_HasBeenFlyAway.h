// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateType.h"
#include "BaseData.h"

#include "BasicFutures_HasBeenFlyAway.generated.h"

class UAbilityTask_TimerHelper;
class UAbilityTask_HasBeenFlyAway;
class UGE_Running;
class UGE_CancelRunning;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_ActiveParam_HasBeenFlyAway : public FGameplayAbilityTargetData_ActiveParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
	);

	virtual FGameplayAbilityTargetData_ActiveParam_HasBeenFlyAway* Clone() const override;

	int32 Height = 100;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ActiveParam_HasBeenFlyAway> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ActiveParam_HasBeenFlyAway>
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
class PLANET_API UBasicFutures_HasBeenFlyAway : public UBasicFuturesBase
{
	GENERATED_BODY()

	using FActiveParamType = FGameplayAbilityTargetData_ActiveParam_HasBeenFlyAway;

public:
	UBasicFutures_HasBeenFlyAway();

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

protected:
	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

private:

	UFUNCTION()
	void OnLanded(
		const FHitResult& Hit

	);

	TSharedPtr<FActiveParamType> ActiveParamSPtr = nullptr;

	ACharacterBase* CharacterPtr = nullptr;
	
	UAbilityTask_HasBeenFlyAway*TaskPtr = nullptr;
};
