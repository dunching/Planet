
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Skill_ZMJZ.generated.h"

class UEffectItem;
class ACharacterBase;

UCLASS()
class PLANET_API USkill_ZMJZ : public USkill_Base
{
	GENERATED_BODY()

public:

	USkill_ZMJZ();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	);

protected:

	virtual void ExcuteStepsLink()override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> BuffIcon;

	float CountDown = 5.f;

	float SecondaryCountDown = 1.f;

	FDelegateHandle AbilityActivatedCallbacksHandle;

	UEffectItem* EffectItemPtr = nullptr;

	uint8 MaxCount = 5;

	uint8 ModifyCount = 0;

	int32 SpeedOffset = 10;

};
