
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_BYWD.generated.h"

struct FCharacterStateInfo;

class ACameraTrailHelper;
class ASPlineActor;
class UAbilityTask_TimerHelper;

UCLASS()
class PLANET_API USkill_Active_BYWD : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:

	void TimerTick(UAbilityTask_TimerHelper*, float Interval);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage1 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Height = 500;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Duration = 3.f;

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr;

};
