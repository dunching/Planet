
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_Slow.generated.h"

class ACameraTrailHelper;
class ASPlineActor;

UCLASS()
class PLANET_API USkill_Active_Slow : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 MoveSpeedOffset = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Duration = 5;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Radius = 250;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Angle = 90;

};
