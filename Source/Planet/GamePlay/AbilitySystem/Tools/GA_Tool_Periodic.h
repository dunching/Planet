
#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"

#include "GA_Tool_Periodic.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;

struct FStreamableHandle;

struct FGameplayAbilityTargetData_Tool_Periodic : public FGameplayAbilityTargetData
{
	TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

	float Duration = 3.f;

	float PerformActionInterval = 1.f;

	TSoftObjectPtr<UTexture2D> DefaultIcon;

	const FGameplayAbilityTargetData_Tool_Periodic& operator=(UConsumableUnit*RightVal);
};

UCLASS()
class PLANET_API UGA_Tool_Periodic : public USkill_Base
{
	GENERATED_BODY()

public:

	UGA_Tool_Periodic();

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

	void UpdateDuration();

protected:

	void PerformAction();

	void ExcuteTasks();

	void OnInterval(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnDuration(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	const FGameplayAbilityTargetData_Tool_Periodic* GameplayAbilityTargetDataPtr = nullptr;

	UEffectItem* EffectItemPtr = nullptr;

	UAbilityTask_TimerHelper* TaskPtr = nullptr;

};
