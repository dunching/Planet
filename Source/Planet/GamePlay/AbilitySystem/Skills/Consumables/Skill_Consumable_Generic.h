// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayAbilitySpecHandle.h"

#include "SceneElement.h"
#include "Skill_Consumable_Base.h"
#include "CharacterAttibutes.h"

#include "Skill_Consumable_Generic.generated.h"

class UAbilityTask_TimerHelper;
class AConsumable_Base;
class UConsumableUnit;
class UGA_Tool_Periodic;
class UGameplayAbility;

UCLASS()
class USkill_Consumable_Generic : public USkill_Consumable_Base
{
	GENERATED_BODY()

public:

	USkill_Consumable_Generic();

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
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	);

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

protected:

	void PerformAction();

	void SpawnActor();

	void ExcuteTasks();

	void PlayMontage();

	void OnPlayMontageEnd();

	void EmitEffect();

	void OnGAEnd(UGameplayAbility* GAPtr);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<AConsumable_Base> Consumable_Class;

	AConsumable_Base* ConsumableActorPtr = nullptr;
	
	UConsumableUnit* UnitPtr = nullptr;
};

struct FGameplayAbilityTargetData_Consumable_Generic : public FGameplayAbilityTargetData
{
	UConsumableUnit* UnitPtr = nullptr;
};
