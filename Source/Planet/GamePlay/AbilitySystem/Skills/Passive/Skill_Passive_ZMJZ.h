
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"
#include "Skill_Passive_Base.h"

#include "Skill_Passive_ZMJZ.generated.h"

struct FStreamableHandle;

class UAbilityTask_TimerHelper;
class UEffectItem;
class ACharacterBase;
class UGE_ZMJZ;
class UGE_ZMJZImp;

struct FGAEventData;
struct FCharacterStateInfo;
struct FReceivedEventModifyDataCallback;

UCLASS()
class PLANET_API USkill_Passive_ZMJZ : public USkill_Passive_Base
{
	GENERATED_BODY()

public:

	using FMakedDamageHandle = 
		TCallbackHandleContainer<void(const FReceivedEventModifyDataCallback&)>::FCallbackHandleSPtr;

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
	)override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	)override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	)override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	)override;

	void OnSendAttack(const FReceivedEventModifyDataCallback& ReceivedEventModifyDataCallback);

	void DurationDelegate(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Duration);

	bool OnTimerTaskFinished(UAbilityTask_TimerHelper* TaskPtr);

	void ModifyCharacterData(
		const FGameplayTag&DataSource,
		int32 Value,
		bool bIsClear = false
	);

	void OnActiveGameplayEffectStackChange(
		FActiveGameplayEffectHandle,
		int32 NewStackCount,
		int32 PreviousStackCount
		);

	void OnGameplayEffectRemoved_InfoDelegate(
		const FGameplayEffectRemovalInfo&
		);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> BuffIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float DecreamTime = 5.f;

	float SecondaryDecreamTime = 1.f;

	FMakedDamageHandle AbilityActivatedCallbacksHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GE")
	TSubclassOf<UGE_ZMJZ>GE_ZMJZClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GE")
	TSubclassOf<UGE_ZMJZImp>GE_ZMJZImpClass;

};
