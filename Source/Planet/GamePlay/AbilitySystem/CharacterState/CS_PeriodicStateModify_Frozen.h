
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "CS_PeriodicStateModify.h"

#include "CS_PeriodicStateModify_Frozen.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_StateModify_Frozen : public FGameplayAbilityTargetData_StateModify
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_StateModify_Frozen();

	FGameplayAbilityTargetData_StateModify_Frozen(
		float Duration
	);

private:

};

UCLASS()
class PLANET_API UCS_PeriodicStateModify_Frozen : public UCS_PeriodicStateModify
{
	GENERATED_BODY()

public:

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);

};
