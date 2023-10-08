// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "GravityPlanet.generated.h"

class USphereComponent;

UCLASS()
class GRAVITY_API AGravityPlanet : public AActor
{
	GENERATED_BODY()
public:

	AGravityPlanet(const FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sphere)
		USphereComponent*SphereComponentPtr = nullptr;

};
