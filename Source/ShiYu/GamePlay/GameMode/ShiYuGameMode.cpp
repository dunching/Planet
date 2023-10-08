// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShiYuGameMode.h"

#include <iostream>

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerInput.h"

#include "Kismet/GameplayStatics.h"

#include "ShiYu/ShiYu.h"
#include "ArticleBase.h"
#include "CollisionDataStruct.h"
#include "SaveGame/ShiYuSaveGame.h"
#include "LogHelper/LogWriter.h"
#include "AssetRefMap.h"
#include "HumanCharacter.h"

AShiYuGameMode::AShiYuGameMode():
	Super()
{
}

void AShiYuGameMode::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AShiYuGameMode::BeginPlay()
{
	Super::BeginPlay();

	LoadGameImp();

	GetWorld()->GetTimerManager().SetTimer(SaveGameTimer, this, &AShiYuGameMode::SaveGameImp, 10.f, true);
}

void AShiYuGameMode::BeginDestroy()
{
	Super::BeginDestroy();
}

void AShiYuGameMode::LoadGameImp()
{
}

void AShiYuGameMode::SaveGameImp()
{
}

void AShiYuGameMode::OnVoxelWorldGenerated()
{

}

void AShiYuGameMode::OnVoxelWorldLoad()
{
}
