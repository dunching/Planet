
#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "PlanetGameplayAbility.h"

#include "Skill_Base.generated.h"

class UBasicUnit;
class UEquipmentElementComponent;

UCLASS()
class PLANET_API USkill_Base : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:

	friend UEquipmentElementComponent;

	USkill_Base();

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
	);

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
	) const;

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	);

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility
	)override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	)override;

	virtual void Tick(float DeltaTime);

	bool GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent)const;

	const TArray<FAbilityTriggerData>& GetTriggers()const;

	void AddCooldownConsumeTime(float NewTime);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Requirement")
	TArray<TSubclassOf<UBasicUnit>>RequirementUnitAry;

	mutable FActiveGameplayEffectHandle CoolDownGEHanlde;

protected:

	virtual void SendEvent(const FGameplayEventData& Payload);

	virtual void ExcuteStepsLink();

	virtual void ExcuteStopStep();

	void OnCurrentStepEnd();

	enum class ERepeatType
	{
		kCount,
		kInfinte,
		kStop,
	};

	ERepeatType RepeatType = ERepeatType::kCount;

	int32 RepeatCount = 1;

	int32 CurrentRepeatCount = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Icon")
	int32 CooldownTime = -1;

	int32 ResetCooldownTime = 1;

protected:

	float CooldownConsumeTime = 0.f;

	ACharacterBase* CharacterPtr = nullptr;

};
