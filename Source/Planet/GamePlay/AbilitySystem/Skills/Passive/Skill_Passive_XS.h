
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"
#include "Skill_Passive_Base.h"


#include "Skill_Passive_XS.generated.h"

struct FCharacterStateInfo;
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

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	)override;

	void ReigsterEffect();

	void AddShield(int32 Value);
	
	void RemoveShield();

	void OnSendAttack(const FGAEventData& GAEventData);

	void CD_DurationDelegate(UAbilityTask_TimerHelper*, float CurrentTime, float Duration);
	
	void Duration_DurationDelegate(UAbilityTask_TimerHelper*, float CurrentTime, float Duration);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> BuffIcon;

	// 生命值低于此百分比的时候触发
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float MinGPPercent = .2f;

	// 可以生成的盾，最大生命值的百分比
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float ShieldValue_HP_Percent = .4f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 CD = 30;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 ShieldDuration = 3;

	// 因为我们给的优先级很高，所以这个数据是结算靠后的，经过了基础的伤害折算
	// 这里的伤害量则为实际的伤害值，这里直接判断伤害是否会让角色HP低于一定阈值
	// struct FMyStruct : public IGAEventModifyReceivedInterface
	// {
	// 	FMyStruct(int32 InPriority, USkill_Passive_XS* InGAInsPtr);
	//
	// 	virtual bool Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override;
	//
	// 	USkill_Passive_XS* GAInsPtr = nullptr;
	// };

	// TSharedPtr<FMyStruct>EventModifyReceivedSPtr = nullptr;

	TSharedPtr<FCharacterStateInfo> CD_CharacterStateInfoSPtr = nullptr;

	TSharedPtr<FCharacterStateInfo> Duration_CharacterStateInfoSPtr = nullptr;
};
