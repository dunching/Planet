
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_Shield.generated.h"

struct FCharacterStateInfo;

class ACameraTrailHelper;
class ASPlineActor;
class UAbilityTask_TimerHelper;

UCLASS()
class PLANET_API USkill_Active_Shield : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	virtual void PerformAction(
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
	)override;

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	);

protected:

	void DurationDelegate(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 PP = 25;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 ShieldValue = 30;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Duration = 5;

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;

};
