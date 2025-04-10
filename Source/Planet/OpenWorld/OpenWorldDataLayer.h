// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "OpenWorldDataLayer.generated.h"

class ATargetPoint;
class ATeleport;
class ASpawnPoints;
class APlanetPlayerController;

enum class EDataLayerRuntimeState : uint8;

UENUM(BlueprintType)
enum class ETeleport : uint8
{
	/**
	 * 从副本回到开放世界
	 */
	kReturnOpenWorld,

	/**
	 * 开放世界中的传送点
	 */
	kTeleport_1,
	
	kTest1,
	kTest2,
	
	/**
	 * 挑战/爬塔系统中的关卡
	 */
	kChallenge_LevelType_1,
	kChallenge_LevelType_2,
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRow_Teleport : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ETeleport ChallengeLevelType = ETeleport::kTest1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ExtendText;

	UPROPERTY(EditAnywhere, Category = "DataLayer")
	TMap<TObjectPtr<const UDataLayerAsset>, EDataLayerRuntimeState> LayerSettingMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<ASpawnPoints> SpawnPointsRef;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<ATeleport> TeleportRef;
	
	/**
	 * 这个是否BegunePlay
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSoftObjectPtr<AActor>> MustReadActors;
};
