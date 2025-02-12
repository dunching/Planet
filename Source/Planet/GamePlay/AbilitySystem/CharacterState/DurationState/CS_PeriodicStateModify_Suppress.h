
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "CS_PeriodicStateModify.h"

#include "CS_PeriodicStateModify_Suppress.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_StateModify_Suppress : 
	public FGameplayAbilityTargetData_StateModify
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_StateModify_Suppress();

	FGameplayAbilityTargetData_StateModify_Suppress(
		float Duration
	);

	FGameplayAbilityTargetData_StateModify_Suppress(
		UAnimMontage* HumanMontagePtr
	);

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual FGameplayAbilityTargetData_StateModify_Suppress* Clone()const override;

	TSharedPtr<FGameplayAbilityTargetData_StateModify_Suppress> Clone_SmartPtr()const;

	UAnimMontage* HumanMontagePtr = nullptr;

private:

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_StateModify_Suppress> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_StateModify_Suppress>
{
	enum
	{
		WithNetSerializer = true,
	};
};

/*
	压制：长度固定，通常会播放对应的蒙太奇
*/
UCLASS()
class PLANET_API UCS_PeriodicStateModify_Suppress : public UCS_PeriodicStateModify
{
	GENERATED_BODY()

public:

	using FStateParam = FGameplayAbilityTargetData_StateModify_Suppress;

	UCS_PeriodicStateModify_Suppress();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

	virtual void UpdateDuration()override;

protected:

	virtual void PerformAction()override;

	// virtual	void InitalDefaultTags()override;

	virtual void OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)override;

	void PlayMontage();

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;

	TSharedPtr<FStateParam> StateParamSPtr = nullptr;

};
