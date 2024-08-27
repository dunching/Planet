
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"
#include "BasicFuturesBase.h"

#include "BasicFutures_Affected.generated.h"

class UAnimMontage;
class ACharacterBase;

struct FGameplayAbilityTargetData_Affected : public FGameplayAbilityTargetData
{
	EAffectedDirection AffectedDirection = EAffectedDirection::kForward;
};

/**
 * 受击时的“僵直效果”
 */

UCLASS()
class PLANET_API UBasicFutures_Affected : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

	UBasicFutures_Affected();

	virtual void PostCDOContruct() override;

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

protected:

	virtual void InitialTags()override;

	void PerformAction(EAffectedDirection AffectedDirection);

	void PlayMontage(UAnimMontage* CurMontagePtr, float Rate);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* ForwardMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* BackwardMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* LeftMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* RightMontage = nullptr;

	ACharacterBase* CharacterPtr = nullptr;

};
