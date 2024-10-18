
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>
#include "AITypes.h"
#include <Navigation/PathFollowingComponent.h>
#include <NavigationSystemTypes.h>

#include "CS_PeriodicStateModify.h"

#include "CS_PeriodicStateModify_Purify.generated.h"

struct FStreamableHandle;
struct FMyPropertySettlementModify;
struct FConsumableProxy;

class UAbilityTask_TimerHelper;
class UTexture2D;
class UEffectItem;
class ASPlineActor;
class ATornado;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_StateModify_Purify :
	public FGameplayAbilityTargetData_StateModify
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_StateModify_Purify();

private:

};

/*
	霸体
*/
UCLASS()
class PLANET_API UCS_PeriodicStateModify_Purify : public UCS_PeriodicStateModify
{
	GENERATED_BODY()

public:

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

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

	virtual void UpdateDuration()override;

protected:

	virtual void PerformAction()override;

	virtual	void InitalTags()override;

	virtual void OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)override;

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;

};
