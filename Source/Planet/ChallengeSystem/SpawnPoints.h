// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/TriggerVolume.h"

#include "SpawnPoints.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UWidgetComponent;
class UBillboardComponent;
class AQuestChain_Branch;

/**
 * 在挑战/爬塔关卡时，可以生成NPC的点位
 */
UCLASS()
class PLANET_API ASpawnPoints : public AActor
{
	GENERATED_BODY()

public:
	ASpawnPoints(const FObjectInitializer& ObjectInitializer);

	TArray<FTransform>GetSpawnPts(int32 Num)const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	TObjectPtr<UBillboardComponent> WidgetComponentPtr = nullptr;
};
