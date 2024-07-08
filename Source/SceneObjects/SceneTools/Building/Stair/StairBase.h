// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BuildingBase.h"

#include "StairBase.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class SCENEOBJECTS_API AStairBase : public ABuildingBase
{
    GENERATED_BODY()

public:

    AStairBase(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	UStaticMeshComponent* GetStaticMeshComponent()const;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPtr")
		UStaticMeshComponent*MeshPtr = nullptr;

};
