
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_Base.h"

#include "CS_PeriodicPropertyModify.generated.h"

class UCS_PeriodicPropertyModify;
class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_PropertyModify : public FGameplayAbilityTargetData_CS_Base
{
	GENERATED_USTRUCT_BODY()

	friend UCS_PeriodicPropertyModify;

	FGameplayAbilityTargetData_PropertyModify();

	FGameplayAbilityTargetData_PropertyModify(
		const FGameplayTag& Tag,
		TSoftObjectPtr<UTexture2D>Icon,
		float InDuration,
		float InPerformActionInterval,
		float InLosePropertyNumInterval,
		const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap
		);

	FGameplayAbilityTargetData_PropertyModify(
		const FGameplayTag& Tag,
		bool bOnluReFreshTime,
		float InDuration,
		float InLosePropertyNumInterval
	);

	FGameplayAbilityTargetData_PropertyModify(
		const FGameplayTag& Tag,bool bClear
	);

	FGameplayAbilityTargetData_PropertyModify(const TSharedPtr<FConsumableProxy>& RightVal);

	virtual FGameplayAbilityTargetData_PropertyModify* Clone()const override;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

private:

	float Duration = 3.f;

	// < 0 为只执行一次 属性修改
	float PerformActionInterval = -1.f;

	// < 0 为移除所有层数
	float LosePropertyNumInterval = -1.f;

	TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

	bool bIsClear = false;

	bool bOnluReFreshTime = false;

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

	void SetCache(const TSharedPtr<FGameplayAbilityTargetData_PropertyModify>& GameplayAbilityTargetDataPtr);

protected:

	virtual void InitialStateDisplayInfo()override;

	virtual void PerformAction();

	void ExcuteTasks();

	void OnInterval(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnDuration(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Duration);

	bool OnTaskFinished_Continue(UAbilityTask_TimerHelper* TaskPtr);

	bool OnTaskFinished(UAbilityTask_TimerHelper* TaskPtr);

	TArray<TSharedPtr<FGameplayAbilityTargetData_PropertyModify>>GameplayAbilityTargetDataAry;

	TSharedPtr<FGameplayAbilityTargetData_PropertyModify>CacheSPtr;

	// 计时Task
	UAbilityTask_TimerHelper* TaskPtr = nullptr;

};
