// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "SpawnHelper.generated.h"

class USphereComponent;

class ARawMaterialBase;

UCLASS()
class PLANETEDITOR_API ASpawnHelper : public AActor
{
	GENERATED_BODY()
public:

	ASpawnHelper(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

protected:

	virtual void SpanActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sphere)
		USphereComponent* SphereComponentPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Fist")
		float Intervals = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Fist")
		float Offset = 10.f;
// 
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Fist")
// 		TMap<TSubclassOf<ARawMaterialBase>, int32>SpawnActorAry;

	FTimerHandle UpdateGravityCenterLocationTimer;

};
