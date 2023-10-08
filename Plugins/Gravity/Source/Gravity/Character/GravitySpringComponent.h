// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/SpringArmComponent.h>

#include "GravitySpringComponent.generated.h"

UCLASS()
class GRAVITY_API UGravitySpringComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sphere)
	float Yaw = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sphere)
    float Pitch = 0.f;

protected:	
	
	uint8 bOrientRotationToMovement:1;

	FRotator GetTargetRotationOverride() const;

	/** Updates the desired arm location, calling BlendLocations to do the actual blending if a trace is done */
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime);

};

