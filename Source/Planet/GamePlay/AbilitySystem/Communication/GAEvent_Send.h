
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"

#include "GAEvent_Send.generated.h"

UCLASS()
class PLANET_API UGAEvent_Send : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:
	UGAEvent_Send();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:

};
