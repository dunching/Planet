// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneObj/Building/BuildingBase.h"

#include "PillarBase.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class SHIYU_API APillarBase : public ABuildingBase
{
    GENERATED_BODY()

public:

	APillarBase(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	UStaticMeshComponent* GetStaticMeshComponent()const;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MeshPtr")
		UStaticMeshComponent*MeshPtr = nullptr;

};
