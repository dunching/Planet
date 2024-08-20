
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"
#include "Skill_Passive_Base.h"

#include "Skill_Passive_ZMJZ.generated.h"

struct FStreamableHandle;

class UAbilityTask_TimerHelper;
class UEffectItem;
class ACharacterBase;

struct FGAEventData;

UCLASS()
class PLANET_API USkill_Passive_ZMJZ : public USkill_Passive_Base
{
	GENERATED_BODY()

public:

	using FMakedDamageHandle = 
		TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>::FCallbackHandleSPtr;

	USkill_Passive_ZMJZ();

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

	virtual void PerformAction()override;

	void OnSendAttack(const FGAEventData& GAEventData);

	void OnIntervalTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> BuffIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float DecreamTime = 5.f;

	float SecondaryDecreamTime = 1.f;

	FMakedDamageHandle AbilityActivatedCallbacksHandle;

	uint8 MaxCount = 5;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 SpeedOffset = 10;

};
