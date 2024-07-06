// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GenerateType.h"
#include "SceneElement.h"

#include "BuildSharedData.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UPrimitiveComponent;
class UChildActorComponent;

class ABuildingBase;

UCLASS(Blueprintable, BlueprintType)
class PLANET_API UCaptureSockComponent : public USceneComponent
{
	GENERATED_BODY()

public:

};

UCLASS(Blueprintable, BlueprintType)
class PLANET_API UCustomChildActorComponent : public UChildActorComponent
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	virtual void PostLoad() override;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EBuildingsType BuildingType = EBuildingsType::kNone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 CaptureOrder = 0;

};

UCLASS()
class PLANET_API ABuildingCaptureData : public AActor
{
	GENERATED_BODY()

public:

	ABuildingCaptureData(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void OnConstruction(const FTransform& Transform)override;

	void StartOverlapCheck(EBuildingsType ItemsType);

	void StopOverlapCheck();

	TSharedPtr<FCapturesInfo> GetCaptureAry(EBuildingsType ItemsType)const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Capture Info")
	UPrimitiveComponent* TerrainCheckComponnetPtr = nullptr;

protected:

	TMap<EBuildingsType, TSharedPtr<FCapturesInfo>>CaptureMap;

};
