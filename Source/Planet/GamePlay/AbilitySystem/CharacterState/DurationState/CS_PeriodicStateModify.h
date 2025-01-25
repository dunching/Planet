
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>
#include "CS_Base.h"

#include "CS_PeriodicStateModify.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_StateModify : public FGameplayAbilityTargetData_CS_Base
{
	GENERATED_USTRUCT_BODY()

	friend UCS_PeriodicStateModify;

	FGameplayAbilityTargetData_StateModify();

	FGameplayAbilityTargetData_StateModify(
		const FGameplayTag& Tag,
		float Duration
	);

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual FGameplayAbilityTargetData_StateModify* Clone()const override;

	TSharedPtr<FGameplayAbilityTargetData_StateModify> Clone_SmartPtr()const;

	// < 0 则意味着由Task/bIsCancelState 取消
	float Duration = 3.f;
	
	// < 0 则意味着由Task取消
	bool bIsCancelState = false;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;
	
private:

};

UCLASS()
class PLANET_API UCS_PeriodicStateModify : public UCS_Base
{
	GENERATED_BODY()

public:

	using FStateParam = FGameplayAbilityTargetData_StateModify;

	UCS_PeriodicStateModify();

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

	virtual void UpdateDuration()override;

	void SetCache(const TSharedPtr<FGameplayAbilityTargetData_StateModify>& GameplayAbilityTargetDataSPtr);

protected:

	virtual void PerformAction();

	void ExcuteTasks();

	virtual void OnInterval(
		UAbilityTask_TimerHelper* TaskPtr,
		float CurrentInterval,
		float Interval
	);

	virtual void OnDuration(
		UAbilityTask_TimerHelper* TaskPtr,
		float CurrentTime, 
		float DurationTime
	);

	virtual void OnTaskTick(
		UAbilityTask_TimerHelper*, 
		float DeltaTime
	);

	void OnTaskComplete();

	TSharedPtr<FGameplayAbilityTargetData_StateModify> GameplayAbilityTargetDataSPtr = nullptr;

	UAbilityTask_TimerHelper* TimerTaskPtr = nullptr;
	
};
