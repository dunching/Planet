
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"
#include "Skill_Passive_Base.h"
#include "GAEvent_Helper.h"

#include "Skill_Passive_XS.generated.h"

struct FStreamableHandle;

class UAbilityTask_TimerHelper;
class UEffectItem;
class ACharacterBase;

struct FGAEventData;

UCLASS()
class PLANET_API USkill_Passive_XS : public USkill_Passive_Base
{
	GENERATED_BODY()

public:

	using FMakedDamageHandle = 
		TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>::FCallbackHandleSPtr;

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

	void PerformAction();

	void OnSendAttack(const FGAEventData& GAEventData);

	void OnIntervalTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> BuffIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float MinGPPercent = .2f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float ShieldValue_HP_Percent = .4f;

	// 因为我们给的优先级很高，所以这个数据是结算靠后的，经过了基础的伤害折算
	// 这里的伤害量则为实际的伤害值，这里直接判断伤害是否会让角色HP低于一定阈值
	struct FMyStruct : public IGAEventModifyReceivedInterface
	{
		FMyStruct(int32 InPriority, USkill_Passive_XS* InGAInsPtr);

		virtual bool Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override;

		USkill_Passive_XS* GAInsPtr = nullptr;
	};

	TSharedPtr<FMyStruct>EventModifyReceivedSPtr = nullptr;

};
