// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "SceneElement.h"

#include "Skill_Active_Base.generated.h"

class UBasicUnit;
class UActiveSkillUnit;
struct FCanbeActivedInfo;

USTRUCT()
struct FGameplayAbilityTargetData_ActiveSkill : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	virtual FGameplayAbilityTargetData_ActiveSkill* Clone()const;

	TSharedPtr<FCanbeActivedInfo> CanbeActivedInfoSPtr;
};

/**
 * ���������Ľ���
 */
UCLASS()
class USkill_Active_Base : public USkill_Base
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

	virtual void Tick(float DeltaTime)override;

	virtual bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const override;

	virtual void AddCooldownConsumeTime(float NewTime);

	// ȷ��������Ŀ���Ƿ��ڷ�Χ��
	bool CheckTargetInDistance(int32 Distance)const;

	// ��ȡ��Χ������ɹ�����Ŀ��
	ACharacterBase* GetTargetInDistance(int32 Distance)const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CooldownTime")
	int32 CooldownTime = -1;

	TSharedPtr<FCanbeActivedInfo> CanbeActivedInfoSPtr;

protected:

	float CooldownConsumeTime = 0.f;

};
