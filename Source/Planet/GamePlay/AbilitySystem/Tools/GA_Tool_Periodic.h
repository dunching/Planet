
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"

#include "GA_Tool_Periodic.generated.h"

struct FGameplayAbilityTargetData_Tool_Periodic : public FGameplayAbilityTargetData
{
	TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

	float Duration = 3.f;

	float PerformActionInterval = 1.f;
};

UCLASS()
class PLANET_API UGA_Tool_Periodic : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Tool_Periodic();

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:

	void PerformAction();

	const FGameplayAbilityTargetData_Tool_Periodic* GameplayAbilityTargetDataPtr = nullptr;

};
