// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MMTPawn.h"

#include "4WheeledVehicleBase.generated.h"

UCLASS()
class GRAVITY_API A4WheeledVehicleBase : public AMMTPawn
{
	GENERATED_BODY()

public:

	A4WheeledVehicleBase(const FObjectInitializer& ObjectInitializer);

 	UFUNCTION(Category = "Pawn|Components|CharacterMovement", BlueprintCallable)
 		virtual FVector GetGravityDirection(bool bAvoidZeroGravity = false) const ;
 
 	// Set a custom gravity direction; use 0, 0, 0 to remove any custom direction.
 	// @note It can be influenced by GravityScale.
 	// @param NewGravityDirection - New gravity direction, assumes it isn't normalize
 	UFUNCTION(Category = "Pawn|Components|CharacterMovement", BlueprintCallable)
 		virtual void SetGravityDirection(FVector NewGravityDirection);

	UFUNCTION(BlueprintNativeEvent, Category = "Move")
		void OnMoveForward(float Value);

	UFUNCTION(BlueprintNativeEvent, Category = "Move")
		void OnMoveRight(float Value);

	virtual void CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance)override;

	virtual USkeletalMeshComponent* GetMesh();

protected:

	virtual void BeginPlay()override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
 		void UpdatePlanetGravity();
 
 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
 		bool HasGravity = false;
 
 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
 		FVector CenterLocation;
 
 	FVector LastAxisZ;
 
 	FQuat LastRotation;
 
 	UPROPERTY()
 		FVector CustomGravityDirection = FVector::ZeroVector;

private:

	FVector GetGravityDirectionInterp(float DeltaSeconds);

	FTimerHandle UpdateGravityCenterLocationTimer;

};