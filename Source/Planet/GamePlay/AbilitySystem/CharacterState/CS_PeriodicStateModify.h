
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"

#include "CS_PeriodicStateModify.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_StateModify : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	friend UCS_PeriodicStateModify;

	FGameplayAbilityTargetData_StateModify();

	FGameplayAbilityTargetData_StateModify(
		const FGameplayTag& Tag,
		float Duration
	);

	FGameplayAbilityTargetData_StateModify* Clone()const;

	// < 0 则意味着由Task取消
	float Duration = 3.f;

	int32 Height = 100;

	// 会一次性修改多个状态码？
	FGameplayTag Tag;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;
	
private:

	TSoftObjectPtr<UTexture2D> DefaultIcon;

};

UCLASS()
class PLANET_API UCS_PeriodicStateModify : public USkill_Base
{
	GENERATED_BODY()

public:

	UCS_PeriodicStateModify();

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

	void OnTaskComplete();

	const FGameplayAbilityTargetData_StateModify* GameplayAbilityTargetDataPtr = nullptr;

	UEffectItem* EffectItemPtr = nullptr;

	UAbilityTask_TimerHelper* TaskPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float FlyAwayHeight = 250.f;

};
