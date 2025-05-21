// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SPlineActor.generated.h"

class USplineComponent;
class UItemProxy_Description_ActiveSkill_Traction;

/**
 *
 */
UCLASS()
class PLANET_API ASPlineActor : public AActor
{
	GENERATED_BODY()

public:

	ASPlineActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TObjectPtr<USplineComponent> SplineComponentPtr;

protected:

	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
	
};
