
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"

#include "GAEvent_Base.generated.h"

UCLASS()
class PLANET_API UGAEvent_Base : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:

	UGAEvent_Base();

	const FGameplayEventData& GetCurrentEventData()const;

protected:

};
