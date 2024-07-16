
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"

#include "GA_Periodic_PropertyModefy.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;

struct FStreamableHandle;

struct FGameplayAbilityTargetData_Periodic_PropertyModefy;

UCLASS()
class PLANET_API UGA_Periodic_PropertyModefy : public USkill_Base
{
	GENERATED_BODY()

public:

	UGA_Periodic_PropertyModefy();

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

	const FGameplayAbilityTargetData_Periodic_PropertyModefy* GameplayAbilityTargetDataPtr = nullptr;

	UEffectItem* EffectItemPtr = nullptr;

	UAbilityTask_TimerHelper* TaskPtr = nullptr;

};

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_Periodic_PropertyModefy : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	friend UGA_Periodic_PropertyModefy;

	FGameplayAbilityTargetData_Periodic_PropertyModefy();

	FGameplayAbilityTargetData_Periodic_PropertyModefy(UConsumableUnit* RightVal);

private:

	TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

	float Duration = 3.f;

	float PerformActionInterval = 1.f;

	TSoftObjectPtr<UTexture2D> DefaultIcon;

};
