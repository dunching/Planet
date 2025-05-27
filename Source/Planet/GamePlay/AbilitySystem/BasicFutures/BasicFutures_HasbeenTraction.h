// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateTypes.h"
#include "BaseData.h"

#include "BasicFutures_HasbeenTraction.generated.h"

class ATractionPoint;
class UAbilityTask_TimerHelper;
class UAbilityTask_HasBeenFlyAway;
class UGE_Running;
class UGE_CancelRunning;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_ActiveParam_HasbeenTraction : public FGameplayAbilityTargetData_ActiveParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
	);

	virtual FGameplayAbilityTargetData_ActiveParam_HasbeenTraction* Clone() const override;
	
	TWeakObjectPtr<ATractionPoint>TractionPoint = nullptr;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ActiveParam_HasbeenTraction> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ActiveParam_HasbeenTraction>
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
class PLANET_API UBasicFutures_HasbeenTraction : public UBasicFuturesBase
{
	GENERATED_BODY()

	using FActiveParamType = FGameplayAbilityTargetData_ActiveParam_HasbeenTraction;

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

protected:
	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:

	TSharedPtr<FActiveParamType> ActiveParamSPtr = nullptr;

	ACharacterBase* CharacterPtr = nullptr;
};
