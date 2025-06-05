
#pragma once

#include "CoreMinimal.h"

#include "GenerateTypes.h"
#include "Skill_Talent_Base.h"

#include "Skill_Talent_NuQi.generated.h"

class UTexture2D;

class FTalent_NuQi;
class ACharacterBase;
class UEffectItem;
class UAbilityTask_TimerHelper;

class FTalent_NuQi : public FTalent_Base
{
public:

	int32 GetCurrentValue() const;

	int32 GetMaxValue() const;

	void SetCurrentValue(int32 val);

	void AddCurrentValue(int32 val);

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

private:

	int32 CurrentValue = 0;

	int32 MaxValue = 100;

};

UCLASS()
class PLANET_API USkill_Talent_NuQi : public USkill_Talent_Base
{
	GENERATED_BODY()

public:

	using FCurrentTalentType = FTalent_NuQi;

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	USkill_Talent_NuQi();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	)override;

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

	virtual void Tick(float DeltaTime);

protected:

	void AddNuQi();

	void SubNuQi(float Inveral);

	void OnHPValueChanged(int32 OldValue, int32 NewValue);

	void StartFuryState();

	void StopFuryState();

	void StartForceWeakState();

	void StopForceWeakState();

	void OnSendDamage(UGameplayAbility* GAPtr);

	bool bIsInFury = false;

	bool bIsInWeak = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float DecrementTime = 3.f;

	float DecrementTime_Accumulate = 0.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float FuryDuration = 10.f;

	float FuryDuration_Accumulate = 0.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> FuryIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float MaxExtendDuration = 5.f;

	float MaxExtendDuration_Accumulate = 0.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float WeakDuration = 5.f;

	float WeakDuration_Accumulate = 0.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> WeakIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 AttackIncrement = 5;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Decrement = 5;

	FValueChangedDelegateHandle OnValueChanged;

	FDelegateHandle AbilityActivatedCallbacksHandle;

	UEffectItem* EffectItemPtr = nullptr;

	float Tick_Interval = 1.f;

	float Tick_Accumulate = 0.f;

	TSharedPtr<FCurrentTalentType>TalentSPtr;

	FGuid PropertuModify_GUID = FGuid::NewGuid();

};
