
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_RootMotion.h"

#include "CS_RootMotion_FlyAway.generated.h"

class UAbilityTask_TimerHelper;
class UAbilityTask_FlyAway;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;
struct FCharacterStateInfo;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion_FlyAway : 
	public FGameplayAbilityTargetData_RootMotion
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_RootMotion_FlyAway();

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual FGameplayAbilityTargetData_RootMotion_FlyAway* Clone()const override;

	float Duration = 3.f;

	int32 Height = 100;

private:

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_RootMotion_FlyAway> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_RootMotion_FlyAway>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class PLANET_API UCS_RootMotion_FlyAway : public UCS_RootMotion
{
	GENERATED_BODY()

public:

	using FRootMotionParam = FGameplayAbilityTargetData_RootMotion_FlyAway;

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

protected:

	// virtual void InitalDefaultTags()override;

	virtual void PerformAction()override;

	virtual void UpdateRootMotionImp(const TSharedPtr<FGameplayAbilityTargetData_RootMotion>& DataSPtr)override;

	void ExcuteTasks();

	void OnInterval(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnDuration(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);

	void OnTaskComplete();

	TSharedPtr<FRootMotionParam>GameplayAbilityTargetDataSPtr;

	UAbilityTask_FlyAway* RootMotionTaskPtr = nullptr;
	
	UAbilityTask_TimerHelper* AbilityTask_TimerHelperPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float FlyAwayHeight = 250.f;

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;

};
