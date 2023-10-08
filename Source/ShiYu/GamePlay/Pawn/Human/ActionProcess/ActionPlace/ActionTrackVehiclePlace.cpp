
#include "ActionTrackVehiclePlace.h"

#include "Kismet/GameplayStatics.h"

#include "GameMode/ShiYuGameMode.h"
#include "ToolsLibrary.h"
#include "DrawDebugHelpers.h"
#include "SceneObj/CollisionDataStruct.h"
#include "Camera/CameraComponent.h"
#include "CharacterBase.h"
#include "SceneObj/Track/TrackBase.h"
#include "SceneObj/Track/TrackVehicleBase.h"
#include "HumanCharacter.h"

namespace HumanProcessor
{
	FActionTrackVehiclePlace::FActionTrackVehiclePlace(AHumanCharacter* CharacterPtr) :
		FPlacingProcessor(CharacterPtr)
	{
	}

	bool FActionTrackVehiclePlace::PrevFindLine(const FHitResult& OutHit, const FVector& StartPt, const FVector& StopPt)
	{
		return true;
	}

	void FActionTrackVehiclePlace::OnPlaceItemPrev()
	{
		bool bIsCanPlace = false;

		// ������Ϊ�գ�����Track���״̬
		auto TrackVehiclePtr = Cast<ATrackVehicleBase>(BuildTargetPtr);
		TrackVehiclePtr->SetAttchedTrack(nullptr);

		FMinimalViewInfo DesiredView;

		GetOwnerActor<AHumanCharacter>()->GetCameraComp()->GetCameraView(0, DesiredView);

		auto StartPt = DesiredView.Location;
		auto StopPt = DesiredView.Location + (DesiredView.Rotation.Vector() * 1000);

		//	DrawDebugLine(PlayerCharacterPtr->GetWorld(), StartPt, StopPt, FColor::Red, false, 1);

		auto OnwerActorPtr = GetOwnerActor<AHumanCharacter>();

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(TrackVehiclePtr);
		ActorsToIgnore.Add(OnwerActorPtr);

		TArray<TEnumAsByte<EObjectTypeQuery>>ObjectTypes;

		FHitResult OutHit;

		if (UKismetSystemLibrary::LineTraceSingleForObjects(
			OnwerActorPtr, StartPt, StopPt,
			ObjectTypes, false, ActorsToIgnore,
			EDrawDebugTrace::ForDuration, OutHit, true, FLinearColor::Red, FLinearColor::Green, 0
		))
		{
			auto TempTrackPtr = Cast<ATrackBase>(OutHit.GetActor());
			if (TempTrackPtr && (TempTrackPtr->CheckTrackBearerState() == FBearerState::kNoVehicleTrack))
			{
				auto CurrentProjectInfo = TempTrackPtr->GetProjectPt(OutHit.ImpactPoint);
				TrackVehiclePtr->SetActorLocation(CurrentProjectInfo.Key.GetLocation());
				TrackVehiclePtr->SetActorRotation(CurrentProjectInfo.Key.GetRotation().Rotator());

				TrackVehiclePtr->SetAttchedTrack(TempTrackPtr);
				TrackVehiclePtr->UpdateCurrentRunLen(CurrentProjectInfo.Value);

				bIsCanPlace = true;

				return;
			}
			TrackVehiclePtr->SetActorLocation(OutHit.ImpactPoint);
			TrackVehiclePtr->SetActorRotation(FRotator::ZeroRotator);
		}

		bIsCanPlace = false;
	}
}