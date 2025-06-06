
#pragma once

#include "CoreMinimal.h"

#include "Skill_WeaponActive_Base.h"

#include "Skill_WeaponActive_HandProtection.generated.h"

class UAnimMontage;

class AWeapon_HandProtection;
class ACharacterBase;
class UAbilityTask_PlayMontage;
class UProgressTips;

struct FGameplayAbilityTargetData_PickAxe;

UCLASS()
class PLANET_API USkill_WeaponActive_HandProtection : public USkill_WeaponActive_Base
{
	GENERATED_BODY()

public:

	USkill_WeaponActive_HandProtection();

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

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	);

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	)override;

	virtual void ResetPreviousStageActions()override;

	void ExcuteStopStep();

	void PlayMontage();

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);
	
	UFUNCTION()
	void OnMontateComplete();

	void MakeDamage();

	void FirstStep();

	void SecondStep();

	void ThirdStep();

	int32 TatalCount = 3;

	int32 CurrentIndex = 0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanStep1Montage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanStep2Montage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanStep3Montage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Distance = 100.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float InputRangeInSecond = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Damage = 7;

	AWeapon_HandProtection* WeaponPtr = nullptr;

};
