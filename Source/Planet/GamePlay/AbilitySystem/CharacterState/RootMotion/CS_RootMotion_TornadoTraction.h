
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_RootMotion.h"

#include "CS_RootMotion_TornadoTraction.generated.h"

class UCapsuleComponent;
class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

UCLASS()
class PLANET_API ATornado : public AActor
{
	GENERATED_BODY()

public:

	ATornado(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 
 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
 	TObjectPtr<UCapsuleComponent> CapsuleComponentPtr = nullptr;

};

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion_TornadoTraction  :
	public FGameplayAbilityTargetData_RootMotion
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_RootMotion_TornadoTraction ();

	virtual FGameplayAbilityTargetData_RootMotion_TornadoTraction * Clone()const override;

	int32 Height = 100;

	TWeakObjectPtr<ATornado> TornadoPtr = nullptr;

private:

};

UCLASS()
class PLANET_API UCS_RootMotion_TornadoTraction : public UCS_RootMotion
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
// 
// 	virtual void UpdateDurationImp()override;

protected:

	virtual void PerformAction()override;

	virtual	void InitalDefaultTags()override;

	void ExcuteTasks();

	void OnInterval(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnDuration(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnTaskComplete();

	const FGameplayAbilityTargetData_RootMotion_TornadoTraction * GameplayAbilityTargetDataPtr = nullptr;
	
	UEffectItem* EffectItemPtr = nullptr;

	UAbilityTask_TimerHelper* TaskPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float FlyAwayHeight = 250.f;

};
