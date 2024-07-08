
#pragma once

#include "CoreMinimal.h"

#include "GAEvent_Base.h"

#include "GAEvent_Send.generated.h"

UCLASS()
class ABILITYSYSTEM_API UGAEvent_Send : public UGAEvent_Base
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
