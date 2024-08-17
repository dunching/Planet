
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_Base.h"

#include "CS_PeriodicPropertyModify.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_PropertyModify : public FGameplayAbilityTargetData_CS_Base
{
	GENERATED_USTRUCT_BODY()

	friend UCS_PeriodicPropertyModify;

	FGameplayAbilityTargetData_PropertyModify();

	FGameplayAbilityTargetData_PropertyModify(
		const FGameplayTag& Tag
	);

	FGameplayAbilityTargetData_PropertyModify(UConsumableUnit* RightVal);

	virtual FGameplayAbilityTargetData_PropertyModify* Clone()const override;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

private:

	TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

	float Duration = 3.f;

	float PerformActionInterval = 1.f;

	FGuid Guid = FGuid::NewGuid();

};

UCLASS()
class PLANET_API UCS_PeriodicPropertyModify : public UCS_Base
{
	GENERATED_BODY()

public:

	UCS_PeriodicPropertyModify();

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

	virtual void InitialStateDisplayInfo()override;

	void PerformAction();

	void ExcuteTasks();

	void OnInterval(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnDuration(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Duration);

	const FGameplayAbilityTargetData_PropertyModify* GameplayAbilityTargetDataPtr = nullptr;

	UAbilityTask_TimerHelper* TaskPtr = nullptr;

};
