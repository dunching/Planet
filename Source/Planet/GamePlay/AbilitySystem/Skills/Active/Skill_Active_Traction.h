
#pragma once

#include "CoreMinimal.h"
#include "ItemDecription.h"
#include "SceneProxyTable.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_Traction.generated.h"

class ATractionPoint;
class ACameraTrailHelper;
class UAnimMontage;
class ASPlineActor;

struct FCharacterStateInfo;

UCLASS()
class PLANET_API UItemProxy_Description_ActiveSkill_Traction : public UItemProxy_Description_ActiveSkill
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float Duration = {3, 4, 5, 6, 7};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float BaseDamage = 100.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float AD = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float AP = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Strength = 300;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 OuterRadius = 200;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bIsPush = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UCurveFloat> StrengthDistanceFalloff;

	float Inverval = 1.f;
};

UCLASS()
class PLANET_API UItemDecription_Skill_Active_Traction : public UItemDecription
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_ActiveSkill_Traction;
private:
	virtual void SetUIStyle() override;
};

/**
 *	牵引效果
 * 以角色为中心，在半径内得敌人会被拖向中心
 * 类似LOL龙王E、天刀丐帮得奴扫乾坤
 */
UCLASS()
class PLANET_API USkill_Active_Traction : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	using FItemProxy_DescriptionType = UItemProxy_Description_ActiveSkill_Traction;
	
	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

protected:

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

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual float GetRemainTime()const override;

	void PlayMontage();

	void OnDuration(UAbilityTask_TimerHelper*, float CurrentTiem, float TotalTime);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontagePtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	FName StartSection = TEXT("Start");

	TObjectPtr<ATractionPoint>TractionPoint = nullptr;

	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;

	float RemainTime = 0.f;
};
