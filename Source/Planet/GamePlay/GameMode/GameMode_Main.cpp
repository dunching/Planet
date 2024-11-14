// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GameMode_Main.h"

#include <iostream>

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Planet.h"
#include "ArticleBase.h"
#include "CollisionDataStruct.h"
#include "SaveGame/ShiYuSaveGame.h"
#include "LogHelper/LogWriter.h"
#include "AssetRefMap.h"
#include "HumanCharacter.h"
#include "WeatherSystem.h"
#include "HoldingItemsComponent.h"
#include "AbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "GameplayTagsSubSystem.h"
#include "ItemProxyContainer.h"
#include "SceneUnitTable.h"
#include "ItemProxy.h"

AGameMode_Main::AGameMode_Main() :
	Super()
{
}

void AGameMode_Main::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AGameMode_Main::BeginPlay()
{
	Super::BeginPlay();

	UWeatherSystem::GetInstance()->RegisterCallback();
	UWeatherSystem::GetInstance()->ResetTime();

	TArray<AActor*>ResultAry;

	LoadGameImp();

	GetWorld()->GetTimerManager().SetTimer(SaveGameTimer, this, &AGameMode_Main::SaveGameImp, 10.f, true);
}

void AGameMode_Main::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto Iter : CharacterUnitMap)
	{
		if (Iter.Value)
		{
			Iter.Value->RelieveRootBind();
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AGameMode_Main::BeginDestroy()
{
	Super::BeginDestroy();
}

void AGameMode_Main::LoadGameImp()
{
}

void AGameMode_Main::SaveGameImp()
{
}

void AGameMode_Main::OnVoxelWorldGenerated()
{

}

void AGameMode_Main::OnVoxelWorldLoad()
{
}

FCharacterProxy* AGameMode_Main::AddCharacterUnit(FGameplayTag UnitType)
{
	return nullptr;
}

FCharacterProxy* AGameMode_Main::FindCharacterUnit(int32 ID)
{
	if (CharacterUnitMap.Contains(ID))
	{
		return CharacterUnitMap[ID];
	}

	return nullptr;
}
