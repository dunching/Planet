// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Element_Base.h"
#include "GenerateType.h"

#include "Skill_Element_Gold.generated.h"

class UAbilityTask_TimerHelper;

struct FBasicProxy;

UCLASS()
class USkill_Element_Gold : public USkill_Element_Base
{
	GENERATED_BODY()

public:

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

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

	void OnElementLevelChanged(int32 OldValue, int32 NewValue);

	void OnSendAttack(UGameplayAbility* GAPtr);

	void AddBuff();
	
	void RemoveBuff();

	FValueChangedDelegateHandle OnValueChanged;

	FDelegateHandle AbilityActivatedCallbacksHandle;

	int32 CurrentElementLevel = 0;

	int32 CurrentBuffLevel = 0;

	UPROPERTY()
	TObjectPtr<UAbilityTask_TimerHelper> RemoveBuffTask;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Property")
	int32 CriticalHitRate = 3;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Property")
	int32 Evade = 3;

	FGuid PropertuModify_GUID = FGuid::NewGuid();

};
