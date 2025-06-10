
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "BasicFuturesBase.generated.h"

class ACharacterBase;

UCLASS()
class PLANET_API UBasicFuturesBase : public UPlanetGameplayAbilityBase
{
	GENERATED_BODY()

public:

	UBasicFuturesBase();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

protected:

	TObjectPtr<ACharacterBase> CharacterPtr = nullptr;

};
