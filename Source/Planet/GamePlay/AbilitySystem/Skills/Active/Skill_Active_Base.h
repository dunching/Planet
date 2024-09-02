// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "SceneElement.h"

#include "Skill_Active_Base.generated.h"

struct FBasicProxy;
struct FActiveSkillProxy;
class UAbilityTask_TimerHelper;
struct FCanbeInteractionInfo;
struct FSkillCooldownHelper;

USTRUCT()
struct FGameplayAbilityTargetData_ActiveSkill : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	virtual FGameplayAbilityTargetData_ActiveSkill* Clone()const;

	TSharedPtr<FCanbeInteractionInfo> CanbeActivedInfoSPtr;

	bool bIsAutoContinue = false;
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
	);

	virtual void Tick(float DeltaTime);

	void GetInputRemainPercent(bool& bIsAcceptInput, float& Percent)const;

	void ContinueActive();

	// 确认是否有锁定的目标
	ACharacterBase* HasFocusActor()const;
	
	// 确认锁定的目标是否在范围内
	bool CheckTargetInDistance(int32 Distance)const;

	// 获取范围内任意可攻击的目标
	ACharacterBase* GetTargetInDistance(int32 Distance)const;

	TSharedPtr<FCanbeInteractionInfo> CanbeActivedInfoSPtr;

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	void CheckInContinue();

	UFUNCTION()
	void WaitInputTick(UAbilityTask_TimerHelper* WaitInputTaskPtr, float Interval, float Duration);

	UAbilityTask_TimerHelper* WaitInputTaskPtr = nullptr;

	bool bIsPreviouInput = false;

	float CurrentWaitInputTime = 3.f;

	float WaitInputPercent = 1.f;

};
