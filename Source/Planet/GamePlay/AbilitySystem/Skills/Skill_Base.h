
#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "PlanetGameplayAbility.h"

#include "Skill_Base.generated.h"

struct FBasicProxy;
struct FSkillProxy;
class UInteractiveComponent;

USTRUCT()
struct FGameplayAbilityTargetData_SkillBase_RegisterParam : 
	public FGameplayAbilityTargetData_RegisterParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	FGuid ProxyID;

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_SkillBase_RegisterParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_SkillBase_RegisterParam>
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

	using FRegisterParamType = FGameplayAbilityTargetData_SkillBase_RegisterParam;

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
	);

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

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
	)override;

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility
	)override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	)override;

	const TArray<FAbilityTriggerData>& GetTriggers()const;

	virtual	void UpdateParam(const FGameplayEventData& GameplayEventData);

protected:

	virtual void ResetPreviousStageActions();

	// 确认是否有锁定的目标
	ACharacterBase* HasFocusActor()const;

	// 确认锁定的目标是否在范围内
	bool CheckTargetInDistance(int32 Distance)const;

	// 获取范围内任意可攻击的目标
	ACharacterBase* GetTargetInDistance(int32 Distance)const;

	ACharacterBase* CharacterPtr = nullptr;

	TSharedPtr<FSkillProxy> SkillUnitPtr = nullptr;

};
