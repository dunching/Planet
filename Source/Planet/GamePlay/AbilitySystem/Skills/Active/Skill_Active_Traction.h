
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_Traction.generated.h"

class ACameraTrailHelper;
class ASPlineActor;

struct FCharacterStateInfo;

UCLASS()
class PLANET_API USkill_Active_Traction : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

protected:

	void IntervalDelegate(
		UAbilityTask_TimerHelper*, 
		float CurrentIntervalTime,
		float IntervalTime,
		bool bIsEnd
	);

	void DurationDelegate(UAbilityTask_TimerHelper*, float CurrentIntervalTime, float IntervalTime);

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 MoveSpeed = 100;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Duration = 5;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Radius = 250;
	
};
