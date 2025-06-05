// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionRadialForce.h"

#include "AbilityTask_ARM_Tornado.generated.h"

DECLARE_DELEGATE(FOnTaskFinished);

class ATornado;

/**
 *	是否应在做成脱手的形式？
 */
UCLASS()
class PLANET_API UAbilityTask_ARM_Tornado :
	public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()

public:
	UAbilityTask_ARM_Tornado(
		const FObjectInitializer& ObjectInitializer
	);

	static UAbilityTask_ARM_Tornado* ApplyRootMotionTornado(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		TWeakObjectPtr<ATornado> TornadoPtr
	);

	FOnTaskFinished OnFinished;

protected:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	virtual void SharedInitAndApply() override;

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void PreDestroyFromReplication() override;
	
	virtual void OnDestroy(bool AbilityIsEnding) override;
	
	UPROPERTY(Replicated)
	TWeakObjectPtr<ATornado> TornadoPtr = nullptr;

private:
};
