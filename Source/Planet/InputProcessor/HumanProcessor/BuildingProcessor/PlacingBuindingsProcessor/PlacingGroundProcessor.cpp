
#include "PlacingGroundProcessor.h"

#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

#include "Character/GravityMovementComponent.h"

#include "GameMode_Main.h"
#include "ToolsLibrary.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "CharacterBase.h"
#include "GenerateType.h"
#include "HumanCharacter.h"

#include "ThreadPoolHelper/ThreadLibrary.h"

#include "BuildSharedData.h"
#include "BuildingBase.h"
#include "CollisionDataStruct.h"

namespace HumanProcessor
{
	FPlacingGroundProcessor::FPlacingGroundProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FPlacingGroundProcessor::EnterAction()
	{
		Super::EnterAction();
	}

	void FPlacingGroundProcessor::QuitAction()
	{
		Super::QuitAction();
	}

	void FPlacingGroundProcessor::AttachActor(const FCaptureInfo& CaptureInfo)
	{
		if (!BuildTargetPtr)
		{
			return;
		}

		// 可以附著時
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