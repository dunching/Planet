
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"

#include "CS_RootMotion.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

struct FGameplayAbilityTargetData_Periodic_RootMotion;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_Periodic_RootMotion : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	friend UCS_RootMotion;

	FGameplayAbilityTargetData_Periodic_RootMotion();

	FGameplayAbilityTargetData_Periodic_RootMotion(
		const FGameplayTag& Tag,
		float Duration
	);

	FGameplayAbilityTargetData_Periodic_RootMotion* Clone()const;

	// < 0 则意味着由Task取消
	float Duration = 3.f;

	int32 Height = 100;

	// 会一次性修改多个状态码？
	FGameplayTag Tag;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;
	
	TWeakObjectPtr<ASPlineActor> SPlineActorPtr = nullptr;
	
	TWeakObjectPtr<ATornado> TornadoPtr = nullptr;

private:

	TSoftObjectPtr<UTexture2D> DefaultIcon;

};

UCLASS()
class PLANET_API UCS_RootMotion : public USkill_Base
{
	GENERATED_BODY()

public:

	UCS_RootMotion();

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

	const FGameplayAbilityTargetData_Periodic_RootMotion* GameplayAbilityTargetDataPtr = nullptr;

	UEffectItem* EffectItemPtr = nullptr;

	UAbilityTask_TimerHelper* TaskPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float FlyAwayHeight = 250.f;

};
