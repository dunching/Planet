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
struct FGameplayAbilityTargetData_ActiveParam_WeaponActive :
	public FGameplayAbilityTargetData_ActiveParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	virtual FGameplayAbilityTargetData_ActiveParam_WeaponActive* Clone()const override;

	bool bIsAutoContinue = false;

	TObjectPtr<AWeapon_Base> WeaponPtr = nullptr;
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ActiveParam_WeaponActive> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ActiveParam_WeaponActive>
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

	using FActiveParamType = FGameplayAbilityTargetData_ActiveParam_WeaponActive;

	USkill_WeaponActive_Base();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	) override;

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

	// virtual	void InitalDefaultTags()override;

	virtual bool GetNum(int32 & Num)const;

protected:
	
	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool CanOncemorePerformAction() const override;

	/**
	 *	进入等待输入，如果按下了攻击键，则继续执行
	 */
	bool PerformIfContinue();

	void EnableMovement(bool bEnableMovement);
	
	FGuid PropertuModify_GUID = FGuid::NewGuid();

	TSharedPtr<FActiveParamType> ActiveParamSPtr = nullptr;

private:

	/**
	 * 攻击前摇结束，等待输入？
	 */
	bool WaitInput = true;

};
