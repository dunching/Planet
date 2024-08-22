// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PlanetGameMode.h"

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
#include "SceneUnitContainer.h"
#include "SceneUnitTable.h"
#include "SceneElement.h"

APlanetGameMode::APlanetGameMode() :
	Super()
{
}

void APlanetGameMode::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APlanetGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWeatherSystem::GetInstance()->RegisterCallback();
	UWeatherSystem::GetInstance()->ResetTime();

	TArray<AActor*>ResultAry;

	LoadGameImp();

	GetWorld()->GetTimerManager().SetTimer(SaveGameTimer, this, &APlanetGameMode::SaveGameImp, 10.f, true);
}

void APlanetGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void APlanetGameMode::BeginDestroy()
{
	Super::BeginDestroy();
}

void APlanetGameMode::LoadGameImp()
{
}

void APlanetGameMode::SaveGameImp()
{
}

void APlanetGameMode::OnVoxelWorldGenerated()
{

}

void APlanetGameMode::OnVoxelWorldLoad()
{
}

UCharacterUnit* APlanetGameMode::AddCharacterUnit(FGameplayTag UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = NewObject<UCharacterUnit>(GetWorld(), SceneUnitExtendInfoPtr->UnitClass);
	//	ResultPtr->AddToRoot();

	const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());

	ResultPtr->ID = NewID;
	ResultPtr->UnitType = UnitType;

	auto SceneUnitContainer = ResultPtr->SceneUnitContainer;

	SceneUnitContainer->AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_Regular, 0);
	SceneUnitContainer->AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_RafflePermanent, 0);
	SceneUnitContainer->AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_RaffleLimit, 0);

	CharacterUnitMap.Add(ResultPtr->ID, ResultPtr);

	return ResultPtr;
}

UCharacterUnit* APlanetGameMode::FindCharacterUnit(int32 ID)
{
	if (CharacterUnitMap.Contains(ID))
	{
		return CharacterUnitMap[ID];
	}

	return nullptr;
}
