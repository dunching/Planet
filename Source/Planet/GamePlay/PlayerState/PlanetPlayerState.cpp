// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "PlanetPlayerState.h"

#include "GeomTools.h"
#include "AbilitySystemComponent.h"
#include "AssetRefMap.h"
#include "Components/AudioComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

#include "CharacterAttributesComponent.h"
#include "GameOptions.h"
#include "InventoryComponent.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"
#include "SceneProxyTable.h"
#include "RegionPromt.h"
#include "MainHUDLayout.h"
#include "Regions.h"
#include "SceneProxyExtendInfo.h"
#include "SplineMesh.h"
#include "UIManagerSubSystem.h"

APlanetPlayerState::APlanetPlayerState(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;

	AudioComponentPtr = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

void APlanetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	InitialData();
}

void APlanetPlayerState::Tick(
	float DeltaSeconds
	)
{
	Super::Tick(DeltaSeconds);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		UpdatePosition();
	}
#endif
}

void APlanetPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APlanetPlayerState::InitialData()
{
}

void APlanetPlayerState::UpdatePosition()
{
	const auto Location = GetPawn()->GetActorLocation();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, ARegionAirWall::StaticClass(), FoundActors);

	TMap<FGameplayTag, TObjectPtr<ARegionAirWall>> RegionAirWallsMap;

	for (auto Iter : FoundActors)
	{
		auto RegionAirWallPtr = Cast<ARegionAirWall>(Iter);
		if (RegionAirWallPtr)
		{
			RegionAirWallsMap.Add(RegionAirWallPtr->RegionTag, RegionAirWallPtr);
		}
	}

	auto AllRegionsPtr = USceneProxyExtendInfoMap::GetInstance()->GetTableRow_AllRegions();
	for (auto RegionPtr : AllRegionsPtr)
	{
		if (!RegionAirWallsMap.Contains(RegionPtr->RegionTag))
		{
			continue;
		}

		auto RegionAirWallPtr = RegionAirWallsMap[RegionPtr->RegionTag];
		if (!RegionAirWallPtr)
		{
			continue;
		}

		TArray<FVector2D> PolygonPoints;

		const auto PointsNum = RegionAirWallPtr->SplineComponentPtr->GetNumberOfSplinePoints();
		for (int32 Index = 0; Index < PointsNum; Index++)
		{
			const auto Transform = RegionAirWallPtr->SplineComponentPtr->GetTransformAtSplinePoint(
				 Index,
				 ESplineCoordinateSpace::World
				);
			PolygonPoints.Add(FVector2D(Transform.GetLocation()));
		}

		if (FGeomTools2D::IsPointInPolygon(FVector2D(Location), PolygonPoints))
		{
			UpdateCurrentPosition(RegionPtr->RegionTag);
			return;
		}
	}

	// 不在设定的区域内，使用默认设定
	UpdateCurrentPosition(UGameplayTagsLibrary::Region_Default);
}

void APlanetPlayerState::UpdateCurrentPosition_Implementation(
	const FGameplayTag& NewCurrentRegionTag
	)
{
	if (CurrentRegionTag == NewCurrentRegionTag)
	{
		return;
	}

	CurrentRegionTag = NewCurrentRegionTag;

	auto TableRow_RegionsPtr = USceneProxyExtendInfoMap::GetInstance()->GetTableRow_Region(NewCurrentRegionTag);
	if (TableRow_RegionsPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_Client)
		{
			AudioComponentPtr->SetSound(TableRow_RegionsPtr->RegionBGM.LoadSynchronous());
			AudioComponentPtr->SetVolumeMultiplier(UGameOptions::GetInstance()->VolumeMultiplier);
			AudioComponentPtr->Play();

			UUIManagerSubSystem::GetInstance()->GetMainHUDLayout()->DisplayWidget(
				 UAssetRefMap::GetInstance()->RegionPromtClass,
				 [TableRow_RegionsPtr](
				 UUserWidget* WidgetPtr
				 )
				 {
					 auto RegionPromtPtr = Cast<URegionPromt>(WidgetPtr);
					 if (RegionPromtPtr)
					 {
						 RegionPromtPtr->SetRegionPromt(TableRow_RegionsPtr);
					 }
				 }
				);
		}
#endif
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_Client)
		{
			AudioComponentPtr->Stop();
		}
#endif
	}
}
