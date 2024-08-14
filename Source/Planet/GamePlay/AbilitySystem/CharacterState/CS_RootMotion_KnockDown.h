
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_RootMotion.h"

#include "CS_RootMotion_KnockDown.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion_KnockDown :
	public FGameplayAbilityTargetData_RootMotion
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_RootMotion_KnockDown();

	FGameplayAbilityTargetData_RootMotion_KnockDown* Clone()const;

private:

};

UCLASS()
class PLANET_API UCS_RootMotion_KnockDown : public UCS_RootMotion
{
	GENERATED_BODY()

public:

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

	virtual void UpdateDuration()override;

protected:

	virtual void PerformAction()override;

	void ExcuteTasks();

	void OnInterval(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnDuration(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnTaskComplete();

	const FGameplayAbilityTargetData_RootMotion_KnockDown* GameplayAbilityTargetDataPtr = nullptr;
	
	UEffectItem* EffectItemPtr = nullptr;

	UAbilityTask_TimerHelper* TaskPtr = nullptr;
	
};
