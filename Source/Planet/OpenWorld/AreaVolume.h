// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/TriggerVolume.h"

#include "AreaVolume.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UWidgetComponent;
class UBillboardComponent;
class AQuestChain_Branch;
class AQuestChain_Area;

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
	TSubclassOf<AQuestChain_Area> GuideBranchThreadClass;

private:
	UFUNCTION()
	void OnActorBeginOverlapCB(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnActorEndOverlapCB(AActor* OverlappedActor, AActor* OtherActor);

	// UPROPERTY(ReplicatedUsing = OnRep_WolrdProcess)
	TObjectPtr<AQuestChain_Area> GuidePtr = nullptr;
};
