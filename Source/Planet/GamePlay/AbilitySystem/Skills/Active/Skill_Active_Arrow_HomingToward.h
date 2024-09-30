
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_Arrow_HomingToward.generated.h"

struct FCharacterStateInfo;

class ACameraTrailHelper;
class ASPlineActor;
class UAbilityTask_TimerHelper;

UCLASS()
class PLANET_API USkill_Active_Arrow_HomingToward : public USkill_Active_Base
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

	void DurationTick(UAbilityTask_TimerHelper*, float Interval, float InDuration);

	bool OnFinished(UAbilityTask_TimerHelper*);

	void SwitchIsHomingToward(bool bIsHomingToward);

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Duration = 5;
	
};
