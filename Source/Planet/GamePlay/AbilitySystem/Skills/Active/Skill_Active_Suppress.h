
#pragma once

#include "CoreMinimal.h"

#include "Skill_Active_Base.h"

#include "Skill_Active_Suppress.generated.h"

class UAnimMontage;

class ATool_PickAxe;
class ACharacterBase;
class ASPlineActor;

struct FGameplayAbilityTargetData_PickAxe;

namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PLANET_API USkill_Active_Suppress : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	using FMoveCompletedSignatureHandle =
		TCallbackHandleContainer<void(EPathFollowingResult::Type)>::FCallbackHandleSPtr;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

protected:

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	);

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);
	
	UFUNCTION(Server, Reliable)
	void PerformAction_Server();

	void PerformActionImp();
	
	UFUNCTION(Server, Reliable)
	void PerformMove_Server(const FVector& StartPt, const FVector& TargetPt);

	void PerformMoveImp(const FVector &StartPt, const FVector& TargetPt);
	
	void ExcuteTasks();
	
	UFUNCTION(Server, Reliable)
	void ExcuteTasksImp(ACharacterBase*TargetCharacterPtr);

	void PlayMontage();

	void MoveCompletedSignature();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* TargetMontage = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Damage = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Radius = 250;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Angle = 90;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 MaxDistance = 300;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Duration = .1f;

	// 在播放动画师角色离目标距离
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Distance = 156;

	FMoveCompletedSignatureHandle MoveCompletedSignatureHandle;

	ATool_PickAxe* EquipmentAxePtr = nullptr;

};
