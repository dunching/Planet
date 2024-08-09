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
// 	UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), AVoxelActor::StaticClass(), ResultAry);
// 	if (!ResultAry.IsEmpty())
// 	{
// 		UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), ACharacterBase::StaticClass(), ResultAry);
// 		for (auto Iter : ResultAry)
// 		{
// 			auto CharacterPtr = Cast<ACharacterBase>(Iter);
// 			if (CharacterPtr)
// 			{
// 				CharacterPtr->GetCharacterMovement()->UpdatedComponent->Mobility = EComponentMobility::Stationary;
// 			}
// 		}
// 
// 		Delegate = GVoxelTaskExecutor->OnEndProcessing.AddLambda([this]() {
// 
// 			TArray<AActor*>ResultAry;
// 			UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), ACharacterBase::StaticClass(), ResultAry);
// 			for (auto Iter : ResultAry)
// 			{
// 				auto CharacterPtr = Cast<ACharacterBase>(Iter);
// 				if (CharacterPtr)
// 				{
// 					CharacterPtr->GetCharacterMovement()->UpdatedComponent->Mobility = EComponentMobility::Movable;
// 				}
// 			}
// 
// 			GVoxelTaskExecutor->OnEndProcessing.Remove(Delegate);
// 			});
// 	}

	LoadGameImp();

	GetWorld()->GetTimerManager().SetTimer(SaveGameTimer, this, &APlanetGameMode::SaveGameImp, 10.f, true);
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
