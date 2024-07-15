// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "KismetGravityLibrary.generated.h"

class USphereComponent;

UCLASS()
class UKismetGravityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static GRAVITY_API FVector GetGravity(const FVector& Location);
};