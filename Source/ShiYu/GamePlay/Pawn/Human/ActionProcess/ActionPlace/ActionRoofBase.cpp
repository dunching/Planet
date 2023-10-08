
#include "ActionRoofBase.h"

#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

#include "Character/GravityMovementComponent.h"

#include "GameMode/ShiYuGameMode.h"
#include "ToolsLibrary.h"
#include "DrawDebugHelpers.h"
#include "SceneObj/CollisionDataStruct.h"
#include "Camera/CameraComponent.h"
#include "CharacterBase.h"
#include "GenerateType.h"
#include "HumanCharacter.h"
#include "CacheAssetManager.h"
#include "SceneObj/Building/BuildSharedData.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ThreadSycAction.h"
#include "SceneObj/Building/Celling/CellBase.h"

namespace HumanProcessor
{
	FActionRoofPlace::FActionRoofPlace(AHumanCharacter* CharacterPtr) :
		FPlacingProcessor(CharacterPtr)
	{
	}

	void FActionRoofPlace::AttachActor(const FCaptureInfo& CaptureInfo)
	{
		if (!BuildTargetPtr)
		{
			return;
		}

		bool bIsAttached = false;

		ON_SCOPE_EXIT
		{
			// 	ThreadLibrary::SyncExecuteInGameThread([&]()
			// 		{
			// 		if (bIsAttached)
			// 		{
			// 			auto VoerlapInfoAry = BuildTargetPtr->GetBuildSharedData()->GetTerrainCheckBoxComponent()->GetOverlapInfos();
			// 			BuildTargetPtr->GetPlaceHelperComponent()->SetCanPlace(VoerlapInfoAry.Num() == 0);
			// 		}
			// 			else
			// 			{
			// 	BuildTargetPtr->GetPlaceHelperComponent()->SetCanPlace(false);
			// }
			// 		});
		};

		// 可以附著時
	// 	if (CaptureInfo.SceneComponentPtr)
	// 	{
	// 		if (CaptureInfo.SceneComponentPtr->GetOwner()->IsA(ABuildingCaptureData::StaticClass()))
	// 		{
	// 			ThreadLibrary::SyncExecuteInGameThread([&]()
	// 				{
	// 					auto Transform = CaptureInfo.SceneComponentPtr->GetRelativeTransform();
	// 					BuildTargetPtr->AttachToActor(CaptureInfo.SceneComponentPtr->GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
	// 					BuildTargetPtr->SetActorRelativeTransform(Transform);
	// 				});
	// 
	// 			bIsAttached = true;
	// 
	// 			return;
	// 		}
	// 	}

		FTransform Transform = FTransform::Identity;
		Transform.SetTranslation(CaptureInfo.HitResult.ImpactPoint);

		auto CharacterPtr = GetOwnerActor<ACharacterBase>();

		const auto GravityOrgin = CharacterPtr->GetGravityMovementComponent()->GravityOrigin;
		auto CharacterForward = CharacterPtr->GetActorForwardVector();
		const auto UpVec = CaptureInfo.HitResult.ImpactPoint - GravityOrgin;

		const auto Rot = FRotationMatrix::MakeFromZX(UpVec, CharacterForward).ToQuat();

		Transform.SetRotation(Rot);

		// 附著到地面
		ThreadLibrary::SyncExecuteInGameThread([&]()
			{
				BuildTargetPtr->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				BuildTargetPtr->SetActorTransform(Transform);
			});
	}
}