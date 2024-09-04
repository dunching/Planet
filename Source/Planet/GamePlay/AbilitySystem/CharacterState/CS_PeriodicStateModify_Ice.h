
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "CS_PeriodicStateModify.h"

#include "CS_PeriodicStateModify_Ice.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_StateModify_Ice : public FGameplayAbilityTargetData_StateModify
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_StateModify_Ice(
		int32 Count = 1,
		float ImmuneTime =8.0f
	);
	
	virtual FGameplayAbilityTargetData_StateModify_Ice* Clone()const override;

	float ImmuneTime = 8.f;
	
	int32 Count = 1;
	
private:

};

UCLASS()
class PLANET_API UCS_PeriodicStateModify_Ice :
	public UCS_PeriodicStateModify
{
	GENERATED_BODY()

public:

	UCS_PeriodicStateModify_Ice();;

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

	int MaxCount = 3;
		
	int CurrentCount = 0;

	bool bIsImmune = false;
	
	void ModifyMaterials();

	void AddTags(const TSharedPtr<FGameplayAbilityTargetData_StateModify_Ice> & CurrentGameplayAbilityTargetDataSPtr);
	
};
