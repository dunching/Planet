#pragma once

#include "CoreMinimal.h"
#include "ItemDecription.h"
#include "SceneProxyTable.h"

#include "Skill_Active_Base.h"

#include "Skill_Active_Tornado.generated.h"

class UPrimitiveComponent;
class UAnimMontage;
class UCapsuleComponent;

class ATool_PickAxe;
class ACharacterBase;
class ASPlineActor;
class UAbilityTask_TimerHelper;
class ATornado;

struct FGameplayAbilityTargetData_PickAxe;

UCLASS()
class PLANET_API UItemProxy_Description_ActiveSkill_Tornado : public UItemProxy_Description_ActiveSkill
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float Duration = {3, 4, 5, 6, 7};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Elemental_Damage = 100.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 MoveSpeed = 300;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 RotationSpeed = 360;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 OuterRadius = 200;

	// 如果这个值过小，会导致移动时距离过近从而忽略本次移动 ?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 InnerRadius = 150;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 MaxHeight = 300;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Height = 300;

	float Inverval = 1.f;
};

UCLASS()
class PLANET_API UItemDecription_Skill_Active_Tornado : public UItemDecription
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_ActiveSkill_Tornado;
private:
	virtual void SetUIStyle() override;
};

struct FGameplayAbilityTargetData_Tornado : public FGameplayAbilityTargetData
{
	ACharacterBase* TargetCharacterPtr = nullptr;
};

UCLASS()
class PLANET_API USkill_Active_Tornado : public USkill_Active_Base
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_ActiveSkill_Tornado;
	
	USkill_Active_Tornado();

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

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	);

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
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

protected:
	virtual void OnGameplayTaskActivated(
		UGameplayTask& Task
	) override;

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	void ExcuteTasks();

	void PlayMontage();

	void OnPlayMontageEnd();

	void OnTimerHelperTick(
		UAbilityTask_TimerHelper* TaskPtr,
		float CurrentInterval,
		float Interval
	);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<ATornado> TornadoClass;

	ATool_PickAxe* EquipmentAxePtr = nullptr;

	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;
};
