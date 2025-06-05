// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/TriggerVolume.h"

#include "AreaVolume.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UWidgetComponent;
class UBillboardComponent;
class AGuideThread_Branch;
class AGuideThread_Area;

/**
 *
 */
UCLASS()
class PLANET_API AAreaVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	AAreaVolume(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "GuideLine")
	TSubclassOf<AGuideThread_Area> GuideBranchThreadClass;

private:
	UFUNCTION()
	void OnActorBeginOverlapCB(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnActorEndOverlapCB(AActor* OverlappedActor, AActor* OtherActor);

	// UPROPERTY(ReplicatedUsing = OnRep_WolrdProcess)
	TObjectPtr<AGuideThread_Area> GuidePtr = nullptr;
};
