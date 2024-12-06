// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "ItemProxy_Minimal.h"

#include "Skill_Active_Base.generated.h"

struct FBasicProxy;
struct FActiveSkillProxy;
class UAbilityTask_TimerHelper;
struct FCanbeInteractionInfo;
struct FSkillCooldownHelper;

USTRUCT()
struct FGameplayAbilityTargetData_ActiveSkill_ActiveParam : 
	public FGameplayAbilityTargetData_ActiveParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual FGameplayAbilityTargetData_ActiveSkill_ActiveParam* Clone()const override;

	bool bIsAutoContinue = false;
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ActiveSkill_ActiveParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ActiveSkill_ActiveParam>
{
	enum
	{
		WithNetSerializer = true,
	};
};

/**
 * 主动触发的节能
 */
UCLASS()
class USkill_Active_Base :
	public USkill_Base
{
	GENERATED_BODY()

public:

	using ActiveParamType = FGameplayAbilityTargetData_ActiveSkill_ActiveParam;

	USkill_Active_Base();

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

	virtual void SetContinuePerformImp(bool bIsContinue)override;

	virtual	void InitalDefaultTags()override;

	virtual void Tick(float DeltaTime);

	// 获取 “等待输入”时长
	void GetInputRemainPercent(bool& bIsAcceptInput, float& Percent)const;
	
protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	// 继续下一段技能
	void ContinueActive();

	// 进入等待输入以进入下一段
	void CheckInContinue();

	// 
	UFUNCTION()
	void WaitInputTick(UAbilityTask_TimerHelper* WaitInputTaskPtr, float Interval, float Duration);
	
	UAbilityTask_TimerHelper* WaitInputTaskPtr = nullptr;

	const ActiveParamType* ActiveParamPtr = nullptr;

	// 是否需要等待输入以继续
	bool bIsPreviouInput = false;

	// 等待输入时常
	float CurrentWaitInputTime = 3.f;

	float WaitInputPercent = 1.f;

};
