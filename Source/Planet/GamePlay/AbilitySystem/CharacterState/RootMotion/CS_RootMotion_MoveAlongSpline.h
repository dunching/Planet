
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_RootMotion.h"

#include "CS_RootMotion_MoveAlongSpline.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion_MoveAlongSpline :
	public FGameplayAbilityTargetData_RootMotion
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_RootMotion_MoveAlongSpline();

	virtual FGameplayAbilityTargetData_RootMotion_MoveAlongSpline* Clone()const override;

	float Duration = 1.f;

	TWeakObjectPtr<ASPlineActor> SPlineActorPtr = nullptr;
	
private:

};

UCLASS()
class PLANET_API UCS_RootMotion_MoveAlongSpline : public UCS_RootMotion
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

// 	virtual void UpdateDurationImp()override;

protected:

	virtual void PerformAction()override;

	void ExcuteTasks();

	void OnInterval(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnDuration(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnTaskComplete();

	const FGameplayAbilityTargetData_RootMotion_MoveAlongSpline* GameplayAbilityTargetDataPtr = nullptr;
	
	UEffectItem* EffectItemPtr = nullptr;

	UAbilityTask_TimerHelper* TaskPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float FlyAwayHeight = 250.f;

};
