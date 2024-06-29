// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ToolUnitBase.h"
#include "PlanetGameplayAbility.h"
#include "ToolFuture_Base.h"

#include "ToolFuture_PickAxe.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;
class ATool_PickAxe;

UCLASS()
class PLANET_API UToolFuture_PickAxe : public UToolFuture_Base
{
	GENERATED_BODY()

public:
	UToolFuture_PickAxe();

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

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility
	);

protected:

	virtual void PerformAction();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* PickAxeMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	ACharacterBase* CharacterPtr = nullptr;

	ATool_PickAxe* EquipmentAxePtr = nullptr;

	bool bIsKeepAction = true;

};

struct FGameplayAbilityTargetData_ToolFuture_PickAxe : public FGameplayAbilityTargetData
{
	ATool_PickAxe* EquipmentAxePtr = nullptr;
};
