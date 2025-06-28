// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"

#include "PlanetGameplayAbility.generated.h"

class UPlanetAbilitySystemComponent;

/**
 * 注册这个GA时附带的参数
 */
USTRUCT()
struct NETABILITYSYSTEM_API FGameplayAbilityTargetData_RegisterParam :
	public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
	);

	virtual FGameplayAbilityTargetData_RegisterParam* Clone() const;

private:
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_RegisterParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_RegisterParam>
{
	enum
	{
		WithNetSerializer = true,
	};
};

/**
 * 激活这个GA时附带的参数
 */
USTRUCT()
struct NETABILITYSYSTEM_API FGameplayAbilityTargetData_ActiveParam :
	public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
	);

	virtual FGameplayAbilityTargetData_ActiveParam* Clone() const;

	int32 ID = 0;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ActiveParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ActiveParam>
{
	enum
	{
		WithNetSerializer = true,
	};
};

/**
 *
 */
UCLASS()
class NETABILITYSYSTEM_API UPlanetGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	using FRegisterParamType = FGameplayAbilityTargetData_RegisterParam;

	friend UPlanetAbilitySystemComponent;

	UPlanetGameplayAbility();

#if WITH_EDITOR
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

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	virtual void OnGameplayTaskInitialized(
		UGameplayTask& Task
	) override;

	virtual void OnGameplayTaskActivated(
		UGameplayTask& Task
	) override;

	virtual void OnGameplayTaskDeactivated(
		UGameplayTask& Task
	) override;

#endif

	virtual void UpdateRegisterParam(
		const FGameplayEventData& GameplayEventData
		);

protected:
	UFUNCTION(Server, Reliable)
	void CancelAbility_Server();

	void RunIfListLock() const;

	void ResetListLock() const;

	void DecrementToZeroListLock() const;

	void DecrementListLockOverride() const;

	/**
	 * GA是否可以要继续执行，比如激活平A时，鼠标左键不妨，此值就是true，松开后则为false
	 * @return 
	 */
	bool GetIsContinue() const;

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);
	
private:
	/**
	 * GA是否可以再次/继续执行，比如当平A的前摇完成时，此函数应该返回true
	 * @return 
	 */
	virtual bool CanOncemorePerformAction() const;

	virtual void PerformActionWrap(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);
	
	void SetContinuePerform(
		bool bIsContinue
	);

	virtual void OnStopContinuePerform();
	
	/**
	 * GA是否持续执行
	 * 通用的属性
	 * 如 我们的武器，按住攻击键时，会不停的执行攻击（前摇+攻击），当松开后，会把攻击执行完（前摇+攻击+后摇）
	 *
	 * 我们的思路是：武器开始攻击时调用 PerformAction 设置变量为true，松开攻击键设为false，
	 * 在前要结束、攻击完成之前，再次按下时，设置变量为false，在前摇结束时检查此变量是否为true；
	 * 在前要结束、攻击完成之后，再次按下时，重新执行PerformAction
	 */
	bool bIsContinueAction = false;
};
