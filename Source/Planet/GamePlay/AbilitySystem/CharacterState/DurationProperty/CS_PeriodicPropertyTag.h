
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"

#include "CS_PeriodicPropertyTag.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_TagModify : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	float Duration = -1.f;

	FGameplayTagContainer ActivationOwnedTags;

};

UCLASS()
class PLANET_API UCS_PeriodicPropertyTag : public USkill_Base
{
	GENERATED_BODY()

public:

	UCS_PeriodicPropertyTag();

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);

};
