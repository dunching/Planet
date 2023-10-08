// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/SceneComponent.h"

#include "TrackTransmissionProcessor.generated.h"

class USceneComponent;

UCLASS(ClassGroup = (Gravity), meta = (BlueprintSpawnableComponent))
class GRAVITY_API UTrackTransmissionProcessor : public USceneComponent
{
	GENERATED_BODY()

public:

	UTrackTransmissionProcessor(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay()override;

	UFUNCTION(BlueprintCallable)
		void GetComponentsReference();

	UFUNCTION(BlueprintCallable)
		void PhysicsUpdate(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		FVector GetTrackLinearVelocityVector();

	UFUNCTION(BlueprintCallable)
		void PreCalculateCoefficients();

	UFUNCTION(BlueprintCallable)
		void UpdateAngularVelocity(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		void SetDriveTrainTorque(float DeltaTorque);

	UFUNCTION(BlueprintCallable)
		float ApplyBrakes(float AngularVelocityIn, float DeltaTime);

	UFUNCTION(BlueprintCallable)
		void DebugSprocket();

	UFUNCTION(BlueprintCallable)
		void UpdateTorque();

	UFUNCTION(BlueprintCallable)
		FVector GetTrackDriveForce(const FVector& TrackForce);

	UFUNCTION(BlueprintCallable)
		void SetTrackApplicationForce(const FVector& FrictionReactionForce, float RollingFrictionForce);

	UFUNCTION(BlueprintCallable)
		void SetBrakeRatio(float NewBrakeRatio);

	UFUNCTION(BlueprintCallable)
		float ApplyMechanicalFriction(float NewAngularVelocity, float DeltaAngularVelocity, float DeltaTime);

	UFUNCTION(BlueprintCallable)
		float ApplyRollingFriction(float AngularVelocityIn, float DeltaTime);

	UFUNCTION(BlueprintCallable)
		float GetTrackLinearVelocityFloat();

	UFUNCTION(BlueprintCallable)
		float GetSprocketAngularVelocity();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Camera)
		USceneComponent* ReferenceFrameComponentMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		FString ReferenceFrameComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float SprocketRadiusCm = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float SprocketMass = 65;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float TrackMass = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float BrakeForce = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float MechanicalFrictionStaticLimit = 0.15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float MechanicalFrictionKineticCoefficient = 0.8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		float AngularVelocity = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Value")
		float SprocketTorque = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Value")
		float MomentOfInertia = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Value")
		float BrakeRatio = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Value")
		float DriveTrainTorque = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Value")
		float TrackContactFrictionTorque = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Value")
		float RollingFrictionTorque = 0;

};