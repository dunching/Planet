
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "CS_PeriodicStateModify.h"

#include "CS_PeriodicStateModify_Slow.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_StateModify_Slow : public FGameplayAbilityTargetData_StateModify
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_StateModify_Slow();

	FGameplayAbilityTargetData_StateModify_Slow(
		int32 InSpeedOffset
	);

	virtual FGameplayAbilityTargetData_StateModify_Slow* Clone()const override;

	int32 SpeedOffset = 0;

private:

};

/*
	被多个减速效果影响时，会使用“减速值”最高的那一个
*/
UCLASS()
class PLANET_API UCS_PeriodicStateModify_Slow :
	public UCS_PeriodicStateModify
{
	GENERATED_BODY()

public:

	UCS_PeriodicStateModify_Slow();;

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

	virtual void OnDuration(
		UAbilityTask_TimerHelper* TaskPtr,
		float CurrentInterval,
		float Interval
	)override;

	void OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime);

	struct FMyStruct
	{
		TSharedPtr<FGameplayAbilityTargetData_StateModify>SPtr;

		float RemainTime = 0.f;
	};

	// 减速速率,源,剩餘持续时间
	TMap<int32, FMyStruct>MoveSpeedOffsetMap;

};
