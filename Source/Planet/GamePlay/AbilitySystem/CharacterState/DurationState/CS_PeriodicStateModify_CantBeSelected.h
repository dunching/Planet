
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "CS_PeriodicStateModify.h"

#include "CS_PeriodicStateModify_CantBeSelected.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FCharacterStateInfo;
struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_StateModify_CantBeSelected : public FGameplayAbilityTargetData_StateModify
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_StateModify_CantBeSelected();

	FGameplayAbilityTargetData_StateModify_CantBeSelected(
		float Duration
	);

private:

};
/*
	使角色进入不可选中的状态，参考 吸血鬼W
*/
UCLASS()
class PLANET_API UCS_PeriodicStateModify_CantBeSelected : public UCS_PeriodicStateModify
{
	GENERATED_BODY()

public:

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	);

	virtual	void InitalDefaultTags()override;

	virtual void PerformAction()override;

	void OnDuration(
		UAbilityTask_TimerHelper* TaskPtr,
		float CurrentTime,
		float DurationTime
	);

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;

};
