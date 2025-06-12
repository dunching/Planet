// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PlanetPlayerState.h"

#include "Components/AudioComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "CharacterAttributesComponent.h"
#include "GameOptions.h"
#include "InventoryComponent.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"
#include "SceneProxyTable.h"
#include "RegionPromt.h"
#include "MainHUDLayout.h"
#include "Regions.h"
#include "DataTableCollection.h"
#include "Dynamic_WeatherBase.h"
#include "GameState_Main.h"
#include "OpenWorldSystem.h"
#include "SplineMesh.h"
#include "UIManagerSubSystem.h"
#include "WeatherSystem.h"
#include "GeomTools.h"
#include "AbilitySystemComponent.h"
#include "AssetRefMap.h"

APlanetPlayerState::APlanetPlayerState(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;

	AudioComponentPtr = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

bool APlanetPlayerState::GetIsInChallenge() const
{
	return bIsInChallenge;
}

FGameplayTag APlanetPlayerState::GetRegionTag() const
{
	return CurrentRegionTag;
}

void APlanetPlayerState::SetCurrentTeleport(
	ETeleport InCurrentTeleport
	)
{
	CurrentTeleport = InCurrentTeleport;
}

void APlanetPlayerState::SetEntryChanlleng_Implementation(
	bool bIsEntryChanlleng
	)
{
	bIsInChallenge = bIsEntryChanlleng;
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

void APlanetPlayerState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, CurrentRegionTag, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bIsInChallenge, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentTeleport, COND_None);
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

	auto AllRegionsPtr = UDataTableCollection::GetInstance()->GetTableRow_AllRegions();
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

void APlanetPlayerState::OnRep_RegionTag()
{
	OnRegionChanged(CurrentRegionTag);

	auto TableRow_RegionsPtr = UDataTableCollection::GetInstance()->GetTableRow_Region(CurrentRegionTag);
	if (TableRow_RegionsPtr)
	{
		AudioComponentPtr->SetSound(TableRow_RegionsPtr->RegionBGM.LoadSynchronous());
		AudioComponentPtr->Play();

		if (UUIManagerSubSystem::GetInstance()->GetMainHUDLayout())
		{
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
	}
	else
	{
		AudioComponentPtr->Stop();
	}
}

void APlanetPlayerState::OnRep_CurrentTeleport()
{
	switch (CurrentTeleport)
	{
	case ETeleport::kChallenge_LevelType_1:
	case ETeleport::kChallenge_LevelType_2:
		{
			auto TeleportDTPtr = UOpenWorldSubSystem::GetInstance()->GetTeleportDT(CurrentTeleport);
			if (!TeleportDTPtr)
			{
				return;
			}

			for (auto Iter : TeleportDTPtr->WeatherTagMap)
			{
				const auto Weather = Iter;
				if (Weather.IsValid())
				{
					// 改变天气
					UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather_Client(Weather);
					break;
				}
			}
		}
		break;
	default:
		{
			auto GSPtr=  Cast<AGameState_Main>(UGameplayStatics::GetGameState(this));

			if (!GSPtr)
			{
				return;
			}
			
			const auto Weather = GSPtr->GetOpenWorldWeather();
			if (!Weather.IsValid())
			{
				// 改变天气
				break;
			}

			UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather_Client(Weather);
		};
	}
}

void APlanetPlayerState::UpdateCurrentPosition(
	const FGameplayTag& NewCurrentRegionTag
	)
{
	if (CurrentRegionTag == NewCurrentRegionTag)
	{
		return;
	}

	CurrentRegionTag = NewCurrentRegionTag;

	OnRegionChanged(CurrentRegionTag);
}
