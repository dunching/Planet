// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BuildingBase.h"

#include "BuildingArea.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UBoxComponent;
class USphereComponent;

UCLASS()
class PLANET_API ABuildingArea : public ABuildingBase
{
	GENERATED_BODY()

public:

	ABuildingArea(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPtr")
	USphereComponent* AreaPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StaticMeshComponent)
	UStaticMeshComponent* MeshPtr = nullptr;

protected:

};
