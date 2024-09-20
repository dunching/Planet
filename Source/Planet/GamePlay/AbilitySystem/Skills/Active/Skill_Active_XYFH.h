
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_XYFH.generated.h"

class ACameraTrailHelper;
class ASPlineActor;

UCLASS()
class PLANET_API USkill_Active_XYFH : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	USkill_Active_XYFH();

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	void ExcuteTasks(
		float StartDistance,
		float EndDistance,
		float Duration,
		bool bIsSubMoveStep
	);

	void PlayMontage();

	void OnPlayMontageEnd();

	void OnMoveStepComplete();

	void OnSubMoveStepComplete();

	UAnimMontage* GetCurrentMontage()const;

	const int32 MaxIndex = 4;

	float SubStepMoveDuration = 0.1f;

	float SubStepPercent = 0.8f;

	UPROPERTY(Replicated)
	int32 StepIndex = 0;
	
	UPROPERTY(Replicated)
	int32 SubStepIndex = 0;
	
	UPROPERTY(Replicated)
	bool bIsContinue = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage1 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage2 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage3 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage4 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<ASPlineActor>SPlineActorClass;

	UPROPERTY(Replicated)
	ASPlineActor* SPlineActorPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<ACameraTrailHelper>CameraTrailHelperClass;
	
	ACameraTrailHelper* CameraTrailHelperPtr = nullptr;

	TScopeValue<FVector> TargetOffsetValue;

};
