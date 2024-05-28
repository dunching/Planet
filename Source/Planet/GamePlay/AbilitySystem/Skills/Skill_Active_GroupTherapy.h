
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"

#include "Skill_Active_GroupTherapy.generated.h"

class UAnimMontage;

class AWeapon_PickAxe;
class ACharacterBase;
class UAbilityTask_PlayMontage;

struct FGameplayAbilityTargetData_PickAxe;

UCLASS()
class PLANET_API USkill_Active_GroupTherapy : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	USkill_Active_GroupTherapy();

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

protected:

	virtual void ExcuteStepsLink()override;

	void PlayMontage();

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);

	void EmitEffect();

	void StartTasksLink();

	bool bIsAttackEnd = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float Radius = 250.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	int32 TreatmentVolume = 5;
	
};