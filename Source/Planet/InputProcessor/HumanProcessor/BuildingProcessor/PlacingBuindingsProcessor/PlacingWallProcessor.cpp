
#include "PlacingWallProcessor.h"

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
	FPlacingWallProcessor::FPlacingWallProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
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