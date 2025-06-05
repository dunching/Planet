
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"

#include "BasicFuturesBase.generated.h"

class ACharacterBase;

UCLASS()
class PLANET_API UBasicFuturesBase : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:

	UBasicFuturesBase();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

protected:

	virtual UGameplayEffect* GetCooldownGameplayEffect() const override;

	virtual UGameplayEffect* GetCostGameplayEffect() const override;

	// virtual void InitalDefaultTags()override;

	TObjectPtr<ACharacterBase> CharacterPtr = nullptr;

};
