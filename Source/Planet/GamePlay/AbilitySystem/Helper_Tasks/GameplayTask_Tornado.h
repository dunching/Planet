// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "GameplayTask_Tornado.generated.h"

class UGameplayAbility;
class ATornado;
class ACharacterBase;

UCLASS()
class PLANET_API UGameplayTask_Tornado : public UGameplayTask
{
	GENERATED_BODY()

public:
	using FOnTaskFinished = TDelegate<void()>;

	UGameplayTask_Tornado(
		const FObjectInitializer& ObjectInitializer
	);

	UPROPERTY(ReplicatedUsing= OnRep_ReplicatedUsing)
	TWeakObjectPtr<ATornado> TornadoPtr = nullptr;
	
protected:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
	) override;

	virtual void OnDestroy(
		bool AbilityIsEnding
	) override;

	virtual void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;

	UFUNCTION()
	void OnRep_ReplicatedUsing();
	
	void PerformAction();
	
	FOnTaskFinished OnFinish;

	uint16 RootMotionSourceID = -1;
};