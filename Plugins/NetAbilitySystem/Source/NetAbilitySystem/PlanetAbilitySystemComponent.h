// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "AbilitySystemComponent.h"

#include "PlanetAbilitySystemComponent.generated.h"

class UPlanetAbilitySystemComponent;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPlanetAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

class NETABILITYSYSTEM_API IPlanetAbilitySystemInterface
{
	GENERATED_BODY()

public:

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const = 0;
	
};

/*
 *	在GA里面使用RPC的NetMulticast蓝图会有警告，所以我们在这里转发一下
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class NETABILITYSYSTEM_API UPlanetAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	virtual void TickComponent(
		float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
	)override;
	
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)override;
	
	virtual void CurrentMontageStop(float OverrideBlendOutTime)override;

	virtual UGameplayAbility* CreateNewInstanceOfAbility(
		FGameplayAbilitySpec& Spec, const UGameplayAbility* Ability
	)override;

	virtual FActiveGameplayEffectHandle ApplyGameplayEffectSpecToTarget(
		const FGameplayEffectSpec& GameplayEffect,
		UAbilitySystemComponent *Target,
		FPredictionKey PredictionKey=FPredictionKey()
		)override;

	UFUNCTION(NetMulticast, Reliable)
	void CurrentMontageStopImp(float OverrideBlendOutTime);
	
	// 我们直接使用 AddLooseGameplayTag ，GAS也会同步到每个连接，但是顺序并不能满足要求，有些地方我们需要用RPC固定顺序
	UFUNCTION(Client, Reliable)
	void AddLooseGameplayTag_2_Client(const FGameplayTag& GameplayTag);
	
	UFUNCTION(Client, Reliable)
	void RemoveLooseGameplayTag_2_Client(const FGameplayTag& GameplayTag);

# pragma region 继续执行的RPC 
	/**
	 * 设置GA为是否持续开启
	 * @param AbilityToTrigger 
	 * @param ActivationInfo 
	 * @param bIsContinue 
	 */
	UFUNCTION(Server, Reliable)
	void SetContinuePerform_Server(
		FGameplayAbilitySpecHandle Handle,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bIsContinue
		);

	/**
	 * 设置 武器 为再次激活
	 * 由ROLE_AutonomousProxy调用
	 * @param Handle 
	 * @param ActivationInfo 
	 */
	UFUNCTION(Server, Reliable)
	void ReplicatePerformAction_Server(
		FGameplayAbilitySpecHandle Handle,
		FGameplayAbilityActivationInfo ActivationInfo
	);
# pragma endregion 

# pragma region 复制参数 
	UFUNCTION(NetMulticast, Reliable)
	void ReplicateEventData(
		int32 InputID,
		const FGameplayEventData& GameplayEventData
	);
	
	UFUNCTION(NetMulticast, Reliable)
	void Replicate_UpdateGAParam(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayEventData&TriggerEventData
	);
# pragma endregion 

	UFUNCTION(BlueprintCallable, Category = "ASC")
	bool K2_HasMatchingGameplayTag(FGameplayTag TagToCheck) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ASC")
	bool K2_HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const;
	
	virtual void ModifyActiveEffectDuration(FActiveGameplayEffectHandle Handle, float Duration);

private:

# pragma region 继续执行的RPC 
	UFUNCTION(NetMulticast, Reliable)
	void ReplicateContinues(
		FGameplayAbilitySpecHandle Handle,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bIsContinue
	);

	UFUNCTION(NetMulticast, Reliable)
	void ReplicatePerformAction(
		FGameplayAbilitySpecHandle Handle,
		FGameplayAbilityActivationInfo ActivationInfo
	);
# pragma endregion 

	TMap<int32, FGameplayEventData>GameplayEventDataMap;

};
