// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Teleport.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UWidgetComponent;
class UBillboardComponent;

/**
 *
 */
UCLASS()
class PLANET_API ATeleport : public AActor
{
	GENERATED_BODY()

public:

	ATeleport(const FObjectInitializer& ObjectInitializer);

	FTransform GetLandTransform() const;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	TObjectPtr<UStaticMeshComponent> StaticComponentPtr = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	TObjectPtr<USceneComponent> LandPtComponentPtr = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> WidgetComponentPtr = nullptr;
#endif
	
};
