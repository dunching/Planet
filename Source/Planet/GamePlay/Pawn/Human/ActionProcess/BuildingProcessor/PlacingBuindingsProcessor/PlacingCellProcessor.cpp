
#include "PlacingCellProcessor.h"

#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

#include "Character/GravityMovementComponent.h"

#include "GameMode/PlanetGameMode.h"
#include "ToolsLibrary.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "CharacterBase.h"
#include "GenerateType.h"
#include "HumanCharacter.h"
#include "CacheAssetManager.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ThreadSycAction.h"
#include "BuildSharedData.h"
#include "BuildingBase.h"
#include "CollisionDataStruct.h"

namespace HumanProcessor
{
	FPlacingCellProcessor::FPlacingCellProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FPlacingCellProcessor::MouseLeftPressed()
	{
		if (bCanPlace)
		{
			if (CaptureInfoSPtr)
			{
				auto BuildingCaptureDataPtr = Cast<ABuildingCaptureData>(CaptureInfoSPtr->CapturePtPtr->GetParentActor());
				if (BuildingCaptureDataPtr)
				{
				}
			}
		}
		Super::MouseLeftPressed(); 
	}

	void FPlacingCellProcessor::AttachActor(const FCaptureInfo& CaptureInfo)
	{
		if (!BuildTargetPtr)
		{
			return;
		}

		// 可以附著時
		CaptureInfoSPtr = CaptureInfo.CaptureInfoSPtr;
		if (CaptureInfo.CaptureInfoSPtr)
		{
			ThreadLibrary::SyncExecuteInGameThread([&]()
				{
					BuildTargetPtr->SetActorTransform(CaptureInfo.CaptureInfoSPtr->CapturePtPtr->GetActorTransform());
				});
		}

		Super::AttachActor(CaptureInfo);
	}
}