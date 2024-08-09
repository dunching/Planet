
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"

#include "CS_AddTemporaryTag.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_AddTemporaryTag : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	friend UGA_AddTemporaryTag;

	FGameplayAbilityTargetData_AddTemporaryTag();

	FGameplayAbilityTargetData_AddTemporaryTag(
		const FGameplayTag& Tag,
		float Duration
	);

	float Duration = -1.f;

	FGameplayTag Tag;

};

UCLASS()
class PLANET_API UGA_AddTemporaryTag : public USkill_Base
{
	GENERATED_BODY()

public:

	UGA_AddTemporaryTag();

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);

};
