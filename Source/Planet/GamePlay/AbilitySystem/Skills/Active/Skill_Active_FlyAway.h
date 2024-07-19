
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"

#include "Skill_Active_FlyAway.generated.h"

class UAnimMontage;

class ATool_PickAxe;
class ACharacterBase;
class ASPlineActor;

struct FGameplayAbilityTargetData_PickAxe;

UCLASS()
class PLANET_API USkill_Active_FlyAway : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	USkill_Active_FlyAway();

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

	void PerformAction();

	void ExcuteTasks();

	void PlayMontage();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	int32 Radius = 250;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	int32 Angle = 90;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float FlyAwayTime = 1.5f;

	ATool_PickAxe* EquipmentAxePtr = nullptr;

};
