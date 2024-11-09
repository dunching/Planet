
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"

#include "Skill_Active_Suppress.generated.h"

class UAnimMontage;

class ATool_PickAxe;
class ACharacterBase;
class ASPlineActor;

struct FGameplayAbilityTargetData_PickAxe;

UCLASS()
class PLANET_API USkill_Active_Suppress : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	void ExcuteTasks();

	void PlayMontage();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* TargetMontage = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Damage = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Radius = 250;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Angle = 90;

	ATool_PickAxe* EquipmentAxePtr = nullptr;

};
