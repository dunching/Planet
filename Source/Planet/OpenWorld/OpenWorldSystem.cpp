#include "OpenWorldSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "Engine/TargetPoint.h"

#include "LogWriter.h"

#include "Planet_Tools.h"
#include "SceneProxyExtendInfo.h"
#include "PlanetPlayerController.h"
#include "Teleport.h"

UOpenWorldSubSystem* UOpenWorldSubSystem::GetInstance()
{
	return Cast<UOpenWorldSubSystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(GetWorldImp(), UOpenWorldSubSystem::StaticClass())
	);
}

void UOpenWorldSubSystem::EntryLevel(ETeleport ChallengeLevelType, APlanetPlayerController* PCPtr)
{
	//在客户端/服务器中，只有服务器可以激活数据层
	if (PCPtr->HasAuthority())
	{
		if (auto DataLayerManagerPtr = UDataLayerManager::GetDataLayerManager(this))
		{
			auto DT_TeleportPtr = USceneProxyExtendInfoMap::GetInstance()->DataTable_Teleport.LoadSynchronous();

			if (ChallengeLevelType == ETeleport::kReturnOpenWorld)
			{
				if (PCPtr->GetPawn()->TeleportTo(OpenWorldTransform.GetLocation(),
				                                 OpenWorldTransform.GetRotation().Rotator()))
				{
				}
			}

			DT_TeleportPtr->ForeachRow<FTableRow_Teleport>(TEXT("GetChallenge"),
				std::bind(&ThisClass::ForeachTeleportRow, this, std::placeholders::_1, std::placeholders::_2, ChallengeLevelType, PCPtr)
				);
		}
	}
}

void UOpenWorldSubSystem::ForeachTeleportRow(
		const FName& Key,
		const FTableRow_Teleport& Value,
		ETeleport ChallengeLevelType,
		APlanetPlayerController* PCPtr
		)
{
	if (auto DataLayerManagerPtr = UDataLayerManager::GetDataLayerManager(this))
	{
		if (Value.ChallengeLevelType == ChallengeLevelType)
		{
			OpenWorldTransform = PCPtr->GetPawn()->
				GetActorTransform();

			if (
				DataLayerManagerPtr->
				SetDataLayerRuntimeState(
					Value.DLS,
					EDataLayerRuntimeState::Activated))
			{
				PRINTINVOKEWITHSTR(
					FString(TEXT("SetDataLayerRuntimeState Success!")
					));
			}
			
			auto TeleportPtr = Value.TeleportRef.LoadSynchronous();
			const auto TeleportTransform = TeleportPtr->GetLandTransform();
			if (PCPtr->GetPawn()->TeleportTo(
				TeleportTransform.GetLocation(), TeleportTransform.GetRotation().Rotator()
				)
			)
			{
				PRINTINVOKEWITHSTR(
					FString(TEXT("Teleport Success!")
					));
			}
		}
		else
		{
			if (
				DataLayerManagerPtr->
				SetDataLayerRuntimeState(
					Value.DLS,
					EDataLayerRuntimeState::Loaded)
			)
			{
			}
		}
	}
}
