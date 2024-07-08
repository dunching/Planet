// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BuildingBase.h"

#include "CellBase.generated.h" 

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class SCENEOBJECTS_API ACellBase : public ABuildingBase
{
	GENERATED_BODY()

public:

	ACellBase(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	UStaticMeshComponent* GetStaticMeshComponent()const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StaticMeshComponent)
		UStaticMeshComponent* MeshPtr = nullptr;

private:

};
