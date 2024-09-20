
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"

#include "GA_Periodic_StateTagModify.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;

struct FStreamableHandle;

struct FGameplayAbilityTargetData_Periodic_StateTagModify;

UCLASS()
class PLANET_API UGA_Periodic_StateTagModify : public USkill_Base
{
	GENERATED_BODY()

public:

	UGA_Periodic_StateTagModify();

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

	const FGameplayAbilityTargetData_Periodic_StateTagModify* GameplayAbilityTargetDataPtr = nullptr;

	UEffectItem* EffectItemPtr = nullptr;

	UAbilityTask_TimerHelper* TaskPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float FlyAwayHeight = 250.f;

};

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_Periodic_StateTagModify : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	friend UGA_Periodic_StateTagModify;

	FGameplayAbilityTargetData_Periodic_StateTagModify();

	FGameplayAbilityTargetData_Periodic_StateTagModify(
		const FGameplayTag& Tag,
		float Duration
	);

	FGameplayAbilityTargetData_Periodic_StateTagModify* Clone()const;

	float Duration = 3.f;

	int32 Height = 100;

	// 会一次性修改多个状态码？
	FGameplayTag Tag;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

private:

	TSoftObjectPtr<UTexture2D> DefaultIcon;

};
