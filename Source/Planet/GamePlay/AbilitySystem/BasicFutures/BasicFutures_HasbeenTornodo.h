// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateType.h"
#include "BaseData.h"

#include "BasicFutures_HasbeenTornodo.generated.h"

class ATornado;
class UAbilityTask_TimerHelper;
class UAbilityTask_HasBeenFlyAway;
class UGE_Running;
class UGE_CancelRunning;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_ActiveParam_HasbeenTornodo : public FGameplayAbilityTargetData_ActiveParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
	);

	virtual FGameplayAbilityTargetData_ActiveParam_HasbeenTornodo* Clone() const override;
	
	TWeakObjectPtr<ATornado>TornadoPtr = nullptr;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ActiveParam_HasbeenTornodo> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ActiveParam_HasbeenTornodo>
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
class PLANET_API UBasicFutures_HasbeenTornodo : public UBasicFuturesBase
{
	GENERATED_BODY()

	using FActiveParamType = FGameplayAbilityTargetData_ActiveParam_HasbeenTornodo;

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
