
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"

#include "GAEvent_Received.generated.h"

UCLASS()
class PLANET_API UGAEvent_Received : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:

	UGAEvent_Received();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:

};
