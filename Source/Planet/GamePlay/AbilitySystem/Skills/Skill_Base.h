
#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "PlanetGameplayAbility.h"

#include "Skill_Base.generated.h"

class UBasicUnit;
class UInteractiveComponent;

UCLASS()
class PLANET_API USkill_Base : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:

	friend UInteractiveComponent;

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

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	);

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

	virtual bool GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent)const;

	const TArray<FAbilityTriggerData>& GetTriggers()const;

protected:

	virtual void ResetPreviousStageActions();

	ACharacterBase* CharacterPtr = nullptr;

};
