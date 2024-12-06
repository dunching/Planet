// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "ItemProxy_Minimal.h"

#include "Skill_WeaponActive_Base.generated.h"

struct FBasicProxy;
class UAbilityTask_TimerHelper;
class AWeapon_Base;

USTRUCT()
struct FGameplayAbilityTargetData_WeaponActive_ActiveParam :
	public FGameplayAbilityTargetData_ActiveParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual FGameplayAbilityTargetData_WeaponActive_ActiveParam* Clone()const override;

	bool bIsAutoContinue = false;

	AWeapon_Base* WeaponPtr = nullptr;
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_WeaponActive_ActiveParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_WeaponActive_ActiveParam>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class USkill_WeaponActive_Base : public USkill_Base
{
	GENERATED_BODY()

public:

	using FActiveParamType = FGameplayAbilityTargetData_WeaponActive_ActiveParam;

	USkill_WeaponActive_Base();

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

	virtual void SetContinuePerformImp(bool bIsContinue)override;

	virtual	void InitalDefaultTags()override;

	virtual bool GetNum(int32 & Num)const;

protected:
	
	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	void ContinueActive();

	void StopContinueActive();

	virtual void CheckInContinue();

	UFUNCTION()
	void WaitInputTick(UAbilityTask_TimerHelper* WaitInputTaskPtr, float Interval, float Duration);

	UAbilityTask_TimerHelper* WaitInputTaskPtr = nullptr;

	bool bIsContinue = true;

	// < 0 则为不限时间，在显式结束前就可以再次输入
	float CurrentWaitInputTime = 1.f;

	float WaitInputPercent = 1.f;

	FGuid PropertuModify_GUID = FGuid::NewGuid();

	TSharedPtr<FActiveParamType> ActiveParamSPtr = nullptr;

private:

};
