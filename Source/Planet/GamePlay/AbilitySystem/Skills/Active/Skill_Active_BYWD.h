#pragma once

#include "CoreMinimal.h"
#include "SceneProxyTable.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_BYWD.generated.h"

struct FCharacterStateInfo;

class ACameraTrailHelper;
class ASPlineActor;
class UAbilityTask_TimerHelper;

UCLASS()
class PLANET_API UItemProxy_Description_ActiveSkill_BYWD : public UItemProxy_Description_ActiveSkill
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float Duration = {3, 4, 5, 6, 7};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Int Height = {450};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 ResingSpeed = 500;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 FallingSpeed = 300;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 SweepOffset = {100};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float DamageFrequency = {1.f, .8f, .6f, .4f, .2f};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Int DamageRadius = {200,};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Int DamageBase = {200,};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float Damage_AD_Magnification = {0.1,};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float Damage_AP_Magnification = {0.1,};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSoftObjectPtr<UAnimMontage> HumanMontageRef = nullptr;
};

/**
 * 类似 逆水寒手游 碧云问笛或天刀 暴雨梨花
 * 会将角色升到空中，并且切换为不可选中状态，期间移速增加，持续时间结束时缓慢下降，着陆之后取消不可选中状态
 */
UCLASS()
class PLANET_API USkill_Active_BYWD : public USkill_Active_Base
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_ActiveSkill_BYWD;

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
		) override;

protected:
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
		) override;

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
		) const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
		) const override;

	virtual float GetRemainTime()const override;

	void PlayMontage();

	void MakeDamage();

	void IntervalDelegate(
		UAbilityTask_TimerHelper*,
		float CurrentIntervalTime,
		float IntervalTime
		);

	void OnDuration(UAbilityTask_TimerHelper*, float CurrentTiem, float TotalTime);
	
	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;
	
	float RemainTime = 0.f;
};
