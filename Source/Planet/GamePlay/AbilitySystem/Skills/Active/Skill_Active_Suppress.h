#pragma once

#include "CoreMinimal.h"
#include "SceneProxyTable.h"

#include "Skill_Active_Base.h"

#include "Skill_Active_Suppress.generated.h"

class UAnimMontage;

class ATool_PickAxe;
class ACharacterBase;
class ASPlineActor;

struct FGameplayAbilityTargetData_PickAxe;

namespace EPathFollowingResult
{
	enum Type : int;
}

UCLASS()
class PLANET_API UItemProxy_Description_ActiveSkill_Suppress : public UItemProxy_Description_ActiveSkill
{
	GENERATED_BODY()

public:
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
	float MoveDuration = .1f;

	// 在播放动画师角色离目标距离
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Distance = 156;
};

/**
 * 压制效果，类似万豪的R或者天刀的饮血技
 * 1.判断范围内是否有目标
 * 2.让目标进入被压制状态
 * 3.快速移动到可以播放蒙太奇的距离内
 * 4.自身和目标播放蒙太奇
 */
UCLASS()
class PLANET_API USkill_Active_Suppress : public USkill_Active_Base
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_ActiveSkill_Suppress;

	using FMoveCompletedSignatureHandle =
	TCallbackHandleContainer<void(
		EPathFollowingResult::Type
		)>::FCallbackHandleSPtr;

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;

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
		) override;

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
		) override;

	void PerformActionImp();

	void PerformMoveImp(
		const FVector& StartPt,
		const FVector& TargetPt
		);

	void ExcuteTasks();

	void PlayMontage();

	void MoveCompletedSignature();

	bool GetOrient(
		FRotator& DesiredRotation,
		bool& bIsImmediatelyRot
		);

	TObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

	FMoveCompletedSignatureHandle MoveCompletedSignatureHandle;

	ATool_PickAxe* EquipmentAxePtr = nullptr;

	FVector TargetLocation;

	FActiveGameplayEffectHandle SuppressGEHandle;

	const int32 GetOrientPrority = 10;

	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;
};
