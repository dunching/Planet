// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/SceneComponent.h"

#include "CombustionEngine.generated.h"

class UCurveFloat;
struct FActorComponentTickFunction;

UCLASS(ClassGroup = (Gravity), meta = (BlueprintSpawnableComponent))
class GRAVITY_API UCombustionEngine : public USceneComponent
{
	GENERATED_BODY()

public:

	UCombustionEngine(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	)override;

	UFUNCTION(BlueprintCallable)
		void UpdateThrottle(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		bool SetThrottle(float Throttle);

	UFUNCTION(BlueprintCallable)
		float GetThrottle();

	UFUNCTION(BlueprintCallable)
		void UpdateEngineTorque();

	UFUNCTION(BlueprintCallable)
		float GetEngineTorque();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float InputThrottle = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float EngineThrottle = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		UCurveFloat* EngineTorqueCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float EngineTorque = 0;

};