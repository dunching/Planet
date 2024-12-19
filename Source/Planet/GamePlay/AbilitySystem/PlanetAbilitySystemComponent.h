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

class PLANET_API IPlanetAbilitySystemInterface
{
	GENERATED_BODY()

public:

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const = 0;
	
};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UPlanetAbilitySystemComponent : public UAbilitySystemComponent
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

	UFUNCTION(NetMulticast, Reliable)
	void CurrentMontageStopImp(float OverrideBlendOutTime);
	
	// 我们直接使用 AddLooseGameplayTag ，GAS也会同步到每个连接，但是顺序并不能满足要求，有些地方我们需要用RPC固定顺序
	UFUNCTION(Client, Reliable)
	void AddLooseGameplayTag_2_Client(const FGameplayTag& GameplayTag);
	
	UFUNCTION(Client, Reliable)
	void RemoveLooseGameplayTag_2_Client(const FGameplayTag& GameplayTag);

	UFUNCTION(Client, Reliable)
	void ReplicateContinues(
		FGameplayAbilitySpecHandle Handle,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bIsContinue
	);

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

	UFUNCTION(BlueprintCallable, Category = "ASC")
	bool K2_HasMatchingGameplayTag(FGameplayTag TagToCheck) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ASC")
	bool K2_HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const;

private:

	TMap<int32, FGameplayEventData>GameplayEventDataMap;

};