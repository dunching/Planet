
#pragma once

#include "CoreMinimal.h"

#include "GenerateType.h"
#include "Skill_Talent_Base.h"

#include "Skill_Talent_YinYang.generated.h"

class UTexture2D;

class FTalent_NuQi;
class ACharacterBase;
class UEffectItem;
class UAbilityTask_TimerHelper;

class FTalent_YinYang : public FTalent_Base
{
public:

	int32 GetCurrentValue() const;

	int32 GetMaxValue() const;

	void SetCurrentValue(int32 val);

	void AddCurrentValue(int32 val);

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

	ETalent_State_Type CurentType = ETalent_State_Type::kYang;

private:

	int32 CurrentValue = 0;

	int32 MaxValue = 100;

};

UCLASS()
class PLANET_API USkill_Talent_YinYang : public USkill_Talent_Base
{
	GENERATED_BODY()

public:

	using FCurrentTalentType = FTalent_YinYang;

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	USkill_Talent_YinYang();

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

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	)override;

	void AddValue(int32 Value);

	void PerformAction_Yang();

	void PerformAction_Yin();

	void OnHPValueChanged(int32 OldValue, int32 NewValue);

	void OnSendDamage(UGameplayAbility* GAPtr);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> YinIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> YangIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 AttackIncrement = 3;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Increment = 1;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Radius = 250.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 TreatmentVolumePercent = 5;

	FValueChangedDelegateHandle OnValueChanged;

	FDelegateHandle AbilityActivatedCallbacksHandle;

	UEffectItem* EffectItemPtr = nullptr;

	float Tick_StateTrans_Interval = 1.f;

	float Tick_StateTrans_Accumulate = 0.f;
	
	float Tick_Buff_Interval = 4.f;

	float Tick_Buff_Accumulate = 0.f;

	TSharedPtr<FCurrentTalentType>TalentSPtr;

	APostProcessVolume* TargetPostPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillType")
	float MaxYang = 7500.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillType")
	float MaxYin = 5500.f;

	float PreviousWhiteTemp = -1.f;

	float WhiteTemp = 6500.f;

};
