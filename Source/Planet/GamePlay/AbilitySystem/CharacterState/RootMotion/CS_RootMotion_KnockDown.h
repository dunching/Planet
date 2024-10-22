
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_RootMotion.h"

#include "CS_RootMotion_KnockDown.generated.h"

class UAbilityTask_TimerHelper;
class UAbilityTask_MyApplyRootMotionConstantForce;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

/*
	击落目标
	仅对处于 Flying或Falling的目标 造成移动效果
*/
USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion_KnockDown :
	public FGameplayAbilityTargetData_RootMotion
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_RootMotion_KnockDown();

	virtual FGameplayAbilityTargetData_RootMotion_KnockDown* Clone()const override;

	int32 KnockDownSpeed = 1000;

private:

};

UCLASS()
class PLANET_API UCS_RootMotion_KnockDown : public UCS_RootMotion
{
	GENERATED_BODY()

public:

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
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

// 	virtual void UpdateDurationImp()override;

protected:

	virtual void PerformAction()override;

	void ExcuteTasks();

	void OnTaskComplete();

	UFUNCTION()
	void OnLanded(const FHitResult& Hit);

	void PlayMontage(UAnimMontage* CurMontagePtr, float Rate);

	void OnMontageComplete();

	const FGameplayAbilityTargetData_RootMotion_KnockDown* GameplayAbilityTargetDataPtr = nullptr;
	
	UAbilityTask_MyApplyRootMotionConstantForce* TaskPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontagePtr = nullptr;

};
