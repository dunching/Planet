
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"

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

	void PerformAction();

	void ExcuteTasks();

	void PlayMontage();

	void OnPlayMontageEnd();

	void OnMoveStepComplete();

	UAnimMontage* GetCurrentMontage()const;

	int32 StepIndex = 0;

	const int32 MaxIndex = 4;

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

	ASPlineActor* SPlineActorPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<ACameraTrailHelper>CameraTrailHelperClass;

	ACameraTrailHelper* CameraTrailHelperPtr = nullptr;

};
