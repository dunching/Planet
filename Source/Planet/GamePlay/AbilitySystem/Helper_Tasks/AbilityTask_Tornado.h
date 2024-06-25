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

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_Tornado* TornadoTask(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		ATornado* InTornadoPtr,
		ACharacterBase* InTargetCharacterPtr
	);

	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	FOnTaskFinished OnFinish;

protected:

	virtual void SharedInitAndApply() override;

protected:

	ATornado* TornadoPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;

};
