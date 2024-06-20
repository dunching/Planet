
#include "PlacingWallProcessor.h"

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
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ThreadSycAction.h"
#include "BuildSharedData.h"
#include "BuildingBase.h"
#include "CollisionDataStruct.h"

namespace HumanProcessor
{
	FPlacingWallProcessor::FPlacingWallProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FPlacingWallProcessor::TKeyPressed()
	{
		switch (WallRot)
		{
		case FPlacingWallProcessor::EWallRot::kRot1:
		{
			WallRot = EWallRot::kRot2;
		}
		break;
		case FPlacingWallProcessor::EWallRot::kRot2:
		{
			WallRot = EWallRot::kRot1;
		}
		break;
		}
	}

	void FPlacingWallProcessor::MouseLeftPressed()
	{
		if (bCanPlace)
		{
			if (CaptureInfoSPtr)
			{
				auto BuildingCaptureDataPtr = Cast<ABuildingCaptureData>(CaptureInfoSPtr->CapturePtPtr->GetParentActor());
				if (BuildingCaptureDataPtr)
				{
// 					auto BulidingPtr = Cast<ABuildingBase>(BuildingCaptureDataPtr);
// 					BuildTargetPtr->SetAttchedBuilding(BulidingPtr);
				}
			}
		}
		Super::MouseLeftPressed();
	}

	void FPlacingWallProcessor::AttachActor(const FCaptureInfo& CaptureInfo)
	{
		if (!BuildTargetPtr)
		{
			return;
		}

		// 可以附著時
		CaptureInfoSPtr = CaptureInfo.CaptureInfoSPtr;
		if (CaptureInfo.CaptureInfoSPtr)
		{
			if (CaptureInfo.CaptureInfoSPtr->CapturePtPtr->IsA(ABuildingBase::StaticClass()))
			{
				FTransform Transform = FTransform::Identity;

				switch (WallRot)
				{
				case ThisClass::EWallRot::kRot1:
				{
					Transform.SetRotation(FQuat(FVector::UpVector, 0.f));
				}
				break;
				case ThisClass::EWallRot::kRot2:
				{
					Transform.SetRotation(FQuat(FVector::UpVector, PI));
				}
				break;
				}

				ThreadLibrary::SyncExecuteInGameThread([&]()
					{
						Transform *= CaptureInfo.CaptureInfoSPtr->CapturePtPtr->GetActorTransform();
						BuildTargetPtr->SetActorTransform(Transform);
					});
			}
		}

		Super::AttachActor(CaptureInfo);
	}
}