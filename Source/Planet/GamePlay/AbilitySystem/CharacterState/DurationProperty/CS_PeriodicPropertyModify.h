
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
struct PLANET_API FGameplayAbilityTargetData_PropertyModify : 
	public FGameplayAbilityTargetData_CS_Base
{
	GENERATED_USTRUCT_BODY()

	friend UCS_PeriodicPropertyModify;

	FGameplayAbilityTargetData_PropertyModify();

	FGameplayAbilityTargetData_PropertyModify(
		const FGameplayTag& Tag,
		TSoftObjectPtr<UTexture2D>Icon,
		float InDuration,
		float InPerformActionInterval,
		const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap
	);

	FGameplayAbilityTargetData_PropertyModify(
		const FGameplayTag& Tag,
		float InDuration,
		float InPerformActionInterval,
		const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap
	);

	FGameplayAbilityTargetData_PropertyModify(const TSharedPtr<FConsumableProxy>& RightVal);

	virtual FGameplayAbilityTargetData_PropertyModify* Clone()const override;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

private:

	float Duration = 3.f;

	// 间隔
	float PerformActionInterval = 1.f;

	TMap<ECharacterPropertyType, FBaseProperty>CharacterPropertyMap;
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

	virtual void PerformAction();

	void ExcuteTasks();

	virtual void OnTaskTick(
		UAbilityTask_TimerHelper*,
		float DeltaTime
	);

	void PerformPropertyModify(const TSharedPtr<FGameplayAbilityTargetData_PropertyModify>& SPtr);

	void OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count);

	// Transient
	TSharedPtr<FGameplayAbilityTargetData_PropertyModify>CacheSPtr;

	struct FMyStruct
	{
		TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr;

		float EffectTime = 0.f;

		TSharedPtr<FGameplayAbilityTargetData_PropertyModify> SPtr;
	};

	// 
	TMap<FGameplayTag, FMyStruct>DurationEffectMap;

	// 计时Task
	UAbilityTask_TimerHelper* TaskPtr = nullptr;

	// 是否使用的净化状态。如果是则移除Debuff类的持续效果
	bool bIsPurify = false;

	FDelegateHandle OnGameplayEffectTagCountChangedHandle;

};
