
#pragma once

#include "CoreMinimal.h"

#include "GAEvent_Base.h"

#include "GAEvent_Received.generated.h"

UCLASS()
class PLANET_API UGAEvent_Received : public UGAEvent_Base
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
