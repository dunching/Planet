// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "SceneElement.h"

#include "Skill_WeaponActive_Base.generated.h"

class UBasicUnit;

UCLASS()
class USkill_WeaponActive_Base : public USkill_Base
{
	GENERATED_BODY()

public:

	USkill_WeaponActive_Base();

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

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

	void ForceCancel();

protected:

	virtual void PerformAction();

	virtual void PerformStopAction();

	virtual bool IsEnd()const;

	void RepeatAction();

	enum class EType
	{
		kNone,
		kRunning,
		kAttackingEnd,
		kFinished,
	};

	EType SkillState = EType::kNone;

	bool bIsRequstCancel = false;

	bool bIsAutomaticStop = false;

	FGuid PropertuModify_GUID = FGuid::NewGuid();

};

struct FGameplayAbilityTargetData_Skill_Weapon : public FGameplayAbilityTargetData
{
	bool bIsAutomaticStop = false;
};
