
#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"

#include "Skill_Displacement.generated.h"

class UAnimMontage;

class ATool_PickAxe;
class ACharacterBase;
class ASPlineActor;

struct FGameplayAbilityTargetData_PickAxe;

UCLASS()
class PLANET_API USkill_Displacement : public USkill_Base
{
	GENERATED_BODY()

public:

	USkill_Displacement();

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

	UFUNCTION()
	void StartTasksLink(int32 ActionNumber);

	void FindTarget();

	void PlayMontage();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float Duration = .5f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float Distance = 800.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float ToCharacterOffset = 100.f;

	ASPlineActor* SPlineActorPtr = nullptr;

	ACharacterBase* CharacterPtr = nullptr;

	ATool_PickAxe* EquipmentAxePtr = nullptr;

};

struct FGameplayAbilityTargetData_Displacement : public FGameplayAbilityTargetData
{
	ACharacterBase* TargetCharacterPtr = nullptr;
};
