#include "OpenWorldSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"

#include "LogWriter.h"

#include "Planet_Tools.h"
#include "SceneProxyExtendInfo.h"
#include "PlanetPlayerController.h"
#include "OpenWorldDataLayer.h"
#include "PlayerGameplayTasks.h"
#include "SpawnPoints.h"
#include "Teleport.h"

UOpenWorldSubSystem* UOpenWorldSubSystem::GetInstance()
{
	return Cast<UOpenWorldSubSystem>(
	                                 USubsystemBlueprintLibrary::GetWorldSubsystem(
		                                  GetWorldImp(),
		                                  UOpenWorldSubSystem::StaticClass()
		                                 )
	                                );
}

bool UOpenWorldSubSystem::ShouldCreateSubsystem(
	UObject* Outer
	) const
{
	// if (!IsRunningDedicatedServer())
	// {
	// 	return false;	
	// }

	return Super::ShouldCreateSubsystem(Outer);
}

ETeleport UOpenWorldSubSystem::GetTeleportLastPtInOpenWorld(
	APlanetPlayerController* PCPtr
	)
{
	ETeleport TargetTeleport = ETeleport::kNone;

	//在客户端/服务器中，只有服务器可以激活数据层
	// if (PCPtr->HasAuthority())
	{
		if (auto DataLayerManagerPtr = UDataLayerManager::GetDataLayerManager(this))
		{
			auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

			TArray<FTableRow_Teleport*> Result;
			DT_TeleportPtr->GetAllRows<FTableRow_Teleport>(TEXT("GetChallenge"), Result);

			const auto PlayerCharacterPt = PCPtr->GetGameplayTasksComponent()->OpenWorldTransform.GetLocation();

			ATeleport* TargetTeleportActorPtr = nullptr;
			int32 Distance = -1;
			for (const auto& RowIter : Result)
			{
				if (!RowIter->bIsOpenWorldTeleport)
				{
					continue;
				}

				if (auto TeleportRef = RowIter->TeleportRef.LoadSynchronous())
				{
					const auto NewDistance = FVector::Dist2D(PlayerCharacterPt, TeleportRef->GetActorLocation());
					if (TargetTeleportActorPtr)
					{
						if (NewDistance < Distance)
						{
							TargetTeleportActorPtr = TeleportRef;
							TargetTeleport = RowIter->ChallengeLevelType;
							Distance = NewDistance;
						}
					}
					else
					{
						TargetTeleportActorPtr = TeleportRef;
						TargetTeleport = RowIter->ChallengeLevelType;
						Distance = NewDistance;
					}
				}
			}
		}
	}

	return TargetTeleport;
}

void UOpenWorldSubSystem::SwitchDataLayer(
	ETeleport ChallengeLevelType,
	APlanetPlayerController* PCPtr
	)
{
	auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

	TArray<FTableRow_Teleport*> Result;
	DT_TeleportPtr->GetAllRows<FTableRow_Teleport>(TEXT("GetChallenge"), Result);

	if (auto DataLayerManagerPtr = UDataLayerManager::GetDataLayerManager(this))
	{
		for (const auto& RowIter : Result)
		{
			if (RowIter->ChallengeLevelType == ChallengeLevelType)
			{
				for (auto DataLayerIter : RowIter->LayerSettingMap)
				{
					if (
						DataLayerManagerPtr->
						SetDataLayerRuntimeState(
						                         DataLayerIter.Key,
						                         DataLayerIter.Value
						                        ))
					{
						PRINTINVOKEWITHSTR(
						                   FString(TEXT("SetDataLayerRuntimeState Success!")
						                   )
						                  );
					}
				}
			}
		}
	}
}

void UOpenWorldSubSystem::TeleportPlayer(
	ETeleport ChallengeLevelType,
	APlanetPlayerController* PCPtr
	)
{
	if (ChallengeLevelType == ETeleport::kReturnOpenWorld)
	{
		if (PCPtr->GetPawn()->TeleportTo(
		                                 PCPtr->GetGameplayTasksComponent()->OpenWorldTransform.GetLocation(),
		                                 PCPtr->GetGameplayTasksComponent()->OpenWorldTransform.GetRotation().Rotator()
		                                ))
		{
		}
	}
	else
	{
		auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

		TArray<FTableRow_Teleport*> Result;
		DT_TeleportPtr->GetAllRows<FTableRow_Teleport>(TEXT("GetChallenge"), Result);

		if (auto DataLayerManagerPtr = UDataLayerManager::GetDataLayerManager(this))
		{
			for (const auto& RowIter : Result)
			{
				if (RowIter->ChallengeLevelType == ChallengeLevelType)
				{
					PCPtr->GetGameplayTasksComponent()->OpenWorldTransform = PCPtr->GetPawn()->
						GetActorTransform();

					auto TeleportPtr = RowIter->TeleportRef.LoadSynchronous();
					const auto TeleportTransform = TeleportPtr->GetLandTransform();
					if (PCPtr->GetPawn()->TeleportTo(
					                                 TeleportTransform.GetLocation(),
					                                 TeleportTransform.GetRotation().Rotator()
					                                )
					)
					{
						PRINTINVOKEWITHSTR(
						                   FString(TEXT("Teleport Success!")
						                   )
						                  );
					}
				}
			}
		}
	}
}

bool UOpenWorldSubSystem::CheckTeleportPlayerComplete(
	ETeleport ChallengeLevelType
	) const
{
	auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

	TArray<FTableRow_Teleport*> Result;
	DT_TeleportPtr->GetAllRows<FTableRow_Teleport>(TEXT("GetChallenge"), Result);

	for (const auto& RowIter : Result)
	{
		if (RowIter->ChallengeLevelType == ChallengeLevelType)
		{
			auto PlayerCharacterPtr = UGameplayStatics::GetPlayerCharacter(this, 0);
			const auto Distance = FVector::Dist2D(
			                                      RowIter->TeleportRef->GetLandTransform().GetLocation(),
			                                      PlayerCharacterPtr->GetActorLocation()
			                                     );

			const auto Threshold = 100;
			if (
				Distance < Threshold
			)
			{
				return true;
			}
		}
	}
	return false;
}

ETeleport UOpenWorldSubSystem::GetTeleportPlayerToNearest(
	APlanetPlayerController* PCPtr
	)
{
	ETeleport TargetTeleport = ETeleport::kNone;

	//在客户端/服务器中，只有服务器可以激活数据层
	// if (PCPtr->HasAuthority())
	{
		if (auto DataLayerManagerPtr = UDataLayerManager::GetDataLayerManager(this))
		{
			auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

			TArray<FTableRow_Teleport*> Result;
			DT_TeleportPtr->GetAllRows<FTableRow_Teleport>(TEXT("GetChallenge"), Result);

			const auto PlayerCharacterPt = PCPtr->GetPawn()->GetActorLocation();

			ATeleport* TargetTeleportActorPtr = nullptr;
			int32 Distance = -1;
			for (const auto& RowIter : Result)
			{
				if (!RowIter->bIsOpenWorldTeleport)
				{
					continue;
				}

				if (auto TeleportRef = RowIter->TeleportRef.LoadSynchronous())
				{
					const auto NewDistance = FVector::Dist2D(PlayerCharacterPt, TeleportRef->GetActorLocation());
					if (TargetTeleportActorPtr)
					{
						if (NewDistance < Distance)
						{
							TargetTeleportActorPtr = TeleportRef;
							TargetTeleport = RowIter->ChallengeLevelType;
							Distance = NewDistance;
						}
					}
					else
					{
						TargetTeleportActorPtr = TeleportRef;
						TargetTeleport = RowIter->ChallengeLevelType;
						Distance = NewDistance;
					}
				}
			}
		}
	}

	return TargetTeleport;
}

bool UOpenWorldSubSystem::CheckSwitchDataLayerComplete(
	ETeleport ChallengeLevelType
	) const
{
	auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

	TArray<FTableRow_Teleport*> Result;
	DT_TeleportPtr->GetAllRows<FTableRow_Teleport>(TEXT("GetChallenge"), Result);

	for (const auto& RowIter : Result)
	{
		if (RowIter->ChallengeLevelType == ChallengeLevelType)
		{
			switch (RowIter->ChallengeLevelType)
			{
			case ETeleport::kReturnOpenWorld:

			default:
				{
					auto LoadTargetPtr = RowIter->TeleportRef.LoadSynchronous();
					if (LoadTargetPtr && LoadTargetPtr->HasActorBegunPlay())
					{
					}
					else
					{
						return false;
					}
					break;
				};
			}

			for (const auto& Iter : RowIter->MustReadActors)
			{
				if (auto ActorPtr = Iter.LoadSynchronous())
				{
					if (!ActorPtr->HasActorBegunPlay())
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}

			switch (RowIter->ChallengeLevelType)
			{
			case ETeleport::kReturnOpenWorld:
			case ETeleport::kTeleport_1:
			case ETeleport::kTest1:
			case ETeleport::kTest2:
				{
					return true;
				}
				break;
			case ETeleport::kChallenge_LevelType_1:
			case ETeleport::kChallenge_LevelType_2:
				{
					auto SpawnPointsRef = RowIter->SpawnPointsRef.LoadSynchronous();
					if (SpawnPointsRef && SpawnPointsRef->HasActorBegunPlay())
					{
						return true;
					}
				}
				break;
			case ETeleport::kChallenge_LevelType_Special_1:
				{
					return true;
				}
				break;
			}
		}
	}

	return false;
}

TSoftObjectPtr<ATeleport> UOpenWorldSubSystem::GetTeleport(
	ETeleport ChallengeLevelType
	) const
{
	auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

	TArray<FTableRow_Teleport*> Result;
	DT_TeleportPtr->GetAllRows<>(TEXT("GetChallenge"), Result);

	for (const auto& RowIter : Result)
	{
		if (RowIter->ChallengeLevelType == ChallengeLevelType)
		{
			return RowIter->TeleportRef;
		}
	}

	return nullptr;
}

FTableRow_Teleport* UOpenWorldSubSystem::GetTeleportDT(
	ETeleport ChallengeLevelType
	) const
{
	auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

	TArray<FTableRow_Teleport*> TeleportResult;
	DT_TeleportPtr->GetAllRows<>(TEXT("GetChallenge"), TeleportResult);

	for (const auto& RowIter : TeleportResult)
	{
		if (RowIter->ChallengeLevelType == ChallengeLevelType)
		{
			return RowIter;
		}
	}

	return nullptr;
}

FGameplayTag UOpenWorldSubSystem::GetTeleportWeather(
	ETeleport ChallengeLevelType
	) const
{
	FGameplayTag NewWeather;
	auto TeleportDTPtr = UOpenWorldSubSystem::GetInstance()->GetTeleportDT(ChallengeLevelType);
	if (TeleportDTPtr->WeatherTagMap.IsEmpty())
	{
		
	}
	else
	{
		int32 Index = FMath::RandRange(0, TeleportDTPtr->WeatherTagMap.Num() - 1);
		int32 CurrentIndex = 0;
		for (const auto& Iter : TeleportDTPtr->WeatherTagMap)
		{
			if (CurrentIndex >= Index)
			{
				NewWeather = Iter;
				break;
			}
			CurrentIndex++;
		}
	}
	return NewWeather; 
}

TArray<FTransform> UOpenWorldSubSystem::GetChallengeSpawnPts(
	ETeleport ChallengeLevelType,
	int32 Num
	) const
{
	TArray<FTransform> Result;

	auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

	TArray<FTableRow_Teleport*> TeleportResult;
	DT_TeleportPtr->GetAllRows<>(TEXT("GetChallenge"), TeleportResult);

	for (const auto& RowIter : TeleportResult)
	{
		if (RowIter->ChallengeLevelType == ChallengeLevelType)
		{
			auto SpawnPointsRef = RowIter->SpawnPointsRef.LoadSynchronous();
			if (SpawnPointsRef)
			{
				Result = SpawnPointsRef->GetSpawnPts(Num);
				break;
			}
			else
			{
				checkNoEntry();
				return Result;
			}
		}
	}

	return Result;
}
