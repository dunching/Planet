#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "PlanetGameplayAbility.h"

#include "GenerateTypes.h"

#include "Skill_Base.generated.h"

struct FBasicProxy;
struct FSkillProxy;
class UInteractiveComponent;
class APlanetWeapon_Base;
class ACharacterBase;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RegisterParam_SkillBase :
	public FGameplayAbilityTargetData_RegisterParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual FGameplayAbilityTargetData_RegisterParam_SkillBase* Clone() const override;

	FGuid ProxyID;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_RegisterParam_SkillBase> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_RegisterParam_SkillBase>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class PLANET_API USkill_Base : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:
	using FRegisterParamType = FGameplayAbilityTargetData_RegisterParam_SkillBase;

	friend UInteractiveComponent;

	USkill_Base();

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
		) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
		) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
		) const override;

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
		) override;

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility
		) override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;

	virtual UGameplayEffect* GetCooldownGameplayEffect() const override;

	virtual UGameplayEffect* GetCostGameplayEffect() const override;

	/**
	 * 
	 * @param AbilityHandle 
	 * @param ActorInfo 
	 * @param ActivationInfo 
	 * @param SpecHandle 
	 * @param TargetData 
	 * @return 
	 */
	TArray<FActiveGameplayEffectHandle> MyApplyGameplayEffectSpecToTarget(
		const FGameplayAbilitySpecHandle AbilityHandle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FGameplayEffectSpecHandle SpecHandle,
		const FGameplayAbilityTargetDataHandle& TargetData
		) const;

	const TArray<FAbilityTriggerData>& GetTriggers() const;

	virtual void UpdateRegisterParam(
		const FGameplayEventData& GameplayEventData
		);

	/**
	 * 获取持续性或具备下一段输入的，不立即进入CD的GA的剩余持续时间
	 * @return > 0 有时间, < 0 无限制时间
	 */
	virtual float GetRemainTime() const;

protected:
	virtual void ResetPreviousStageActions();

	// 确认是否有锁定的目标
	ACharacterBase* HasFocusActor() const;

	// 确认锁定的目标是否在范围内
	bool CheckTargetInDistance(
		int32 Distance
		) const;

	// 确认锁定的目标是否在范围上（需要跟目标保持一定距离）
	bool CheckTargetIsEqualDistance(
		int32 Distance
		) const;

	// 获取范围内任意可攻击的目标
	ACharacterBase* GetTargetInDistance(
		int32 MaxDistance
		) const;

	/**
	 * 获取范围内所有目标，按距离角色远近排序
	 * @param MaxDistance 
	 * @return 
	 */
	TArray<TObjectPtr<ACharacterBase>> GetTargetsInDistanceByNearestCharacter(
		int32 MaxDistance,
		int32 UpForwardDistance,
		int32 DownDistance
		) const;

	/**
	 * 获取范围内所有目标，按距离角色视线排序
	 * @param MaxDistance 
	 * @return 
	 */
	TArray<TObjectPtr<ACharacterBase>> GetTargetsInDistanceByNearestCharacterViewDirection(
		int32 MaxDistance,
		int32 UpForwardDistance,
		int32 DownDistance
		) const;

	FGameplayEffectSpecHandle MakeDamageToTargetSpecHandle(
		EElementalType ElementalType,
		int32 Elemental_Damage,
		float Elemental_Damage_Magnification
		)const;

	TObjectPtr<ACharacterBase> CharacterPtr = nullptr;

	TSharedPtr<FSkillProxy> SkillProxyPtr = nullptr;

private:
	TSet<TObjectPtr<ACharacterBase>> GetTargetsInDistance(
		int32 MaxDistance,
		int32 UpForwardDistance,
		int32 DownDistance
		) const;
};
