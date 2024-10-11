
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_RootMotion.h"

#include "CS_RootMotion_Traction.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class UAbilityTask_MyApplyRootMotionRadialForce;

struct FStreamableHandle;
struct FCharacterStateInfo;

// 参考龙王E
USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion_Traction  :
	public FGameplayAbilityTargetData_RootMotion
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_RootMotion_Traction();

	virtual FGameplayAbilityTargetData_RootMotion_Traction* Clone()const override;

	bool bIsEnd = false;

	int32 MoveSpeed = 100;
	
	// 
	int32 Radius = 100;

	FVector TaretPt = FVector::ZeroVector;

private:

};

UCLASS()
class PLANET_API UCS_RootMotion_Traction : public UCS_RootMotion
{
	GENERATED_BODY()

public:

	using FRootMotionParam = FGameplayAbilityTargetData_RootMotion_Traction;

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

	void SetCache(const TSharedPtr<FRootMotionParam>& GameplayAbilityTargetDataPtr);

protected:

	virtual void PerformAction()override;

	void ExcuteTasks();

	void OnTaskComplete();

	TSharedPtr<FRootMotionParam>GameplayAbilityTargetDataSPtr;

	UAbilityTask_MyApplyRootMotionRadialForce* RootMotionTaskPtr = nullptr;
	
	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;

};
