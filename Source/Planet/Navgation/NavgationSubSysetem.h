// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GenerateTypes.h"
#include <GameInstance/PlanetGameInstance.h>
#include "PlanetModule.h"
#include "ItemProxy_Minimal.h"

#include "NavgationSubSysetem.generated.h"

class ANavMeshBoundsVolume;
class AFlyingNavigationData;
class UNavigationPath;

UCLASS()
class PLANET_API UNavgationSubSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	static UNavgationSubSystem* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

protected:

	UFUNCTION()
	void FlyingNavGenerationFinished();

	void CheckNeedRebuildNavData();

	void RebuildNavData(
		ANavMeshBoundsVolume* NavMeshBoundsVolumePtr, 
		ACharacter*CharacterPtr,
		const FVector& TargetPt
	);

	float Interval = 1.f;

	float CurrentInterval = 0.f;

	AFlyingNavigationData* PreviouFlyingNavigationDataPtr = nullptr;

};
