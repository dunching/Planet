// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"

#include "AbilityTask_Tornado.generated.h"

class UGameplayAbility;
class ATornado;
class ACharacterBase;

DECLARE_DELEGATE(FOnTaskFinished);

UCLASS()
class PLANET_API UAbilityTask_Tornado : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()

public:

	UAbilityTask_Tornado(const FObjectInitializer& ObjectInitializer);

	static UAbilityTask_Tornado* NewTask(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		TWeakObjectPtr<ATornado> InTornadoPtr,
		ACharacterBase* InTargetCharacterPtr
	);

	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	virtual void TickTask(float DeltaTime) override;

	FOnTaskFinished OnFinish;

protected:

	virtual void SharedInitAndApply() override;

protected:

	TWeakObjectPtr<ATornado> TornadoPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;

};
