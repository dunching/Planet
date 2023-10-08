
#include "ActionTrackPlace.h"

#include "Kismet/GameplayStatics.h"

#include "GameMode/ShiYuGameMode.h"
#include "ToolsLibrary.h"
#include "DrawDebugHelpers.h"
#include "SceneObj/CollisionDataStruct.h"
#include "Camera/CameraComponent.h"
#include "CharacterBase.h"
#include "SceneObj/Track/TrackBase.h"
#include "HumanCharacter.h"
#include "InputProcessorSubSystem.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"

namespace HumanProcessor
{
	FActionTrackPlace::FActionTrackPlace(AHumanCharacter* CharacterPtr) :
		FPlacingProcessor(CharacterPtr)
	{
	}

	void FActionTrackPlace::EnterAction()
	{
		FPlacingProcessor::EnterAction();

		LinkTrackPtr = nullptr;
		LinkPos = ETrackVehiclePosState::kFront;
		CurPtIndex = 0;
		FindArea = 100;
	}

	void FActionTrackPlace::MouseLeftPressed()
	{
		if (CurPtIndex == 0)
		{
			CurPtIndex++;
		}
		else if (CurPtIndex == 1)
		{
			if (LinkTrackPtr)
			{
				LinkTrackPtr->SetLinkTrack(TrackPtr, LinkPos);
			}

			if (bCanPlace)
			{
				TrackPtr->GetStateController()->SwitchToNewState(EBuildingState::kPlaced);
				TrackPtr = nullptr;

				UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<FHumanRegularProcessor>();
			}
		}
	}

	bool FActionTrackPlace::PrevFindSphere(const TArray<FHitResult>& OutHitsAry, const FVector& StartPt, const FVector& StopPt)
	{
		int32 Len = -1;
		ATrackBase* NearestTrackPtr = nullptr;

		auto Dir = StartPt - StopPt;

		for (auto Iter : OutHitsAry)
		{
			if (!Iter.GetActor())
			{
				continue;
			}

			auto TempTrackPtr = Cast<ATrackBase>(Iter.GetActor());
			if (!TempTrackPtr)
			{
				continue;
			}

			if (NearestTrackPtr)
			{
				FTransform FirstPt = TempTrackPtr->GetLinkPos(ETrackVehiclePosState::kFront);
				FTransform SecondPt = TempTrackPtr->GetLinkPos(ETrackVehiclePosState::kBack);

				auto TempLen1 = FMath::PointDistToLine(FirstPt.GetLocation(), Dir, StopPt);
				auto TempLen2 = FMath::PointDistToLine(SecondPt.GetLocation(), Dir, StopPt);

				//DrawDebugSphere(Iter.Actor->GetWorld(), FirstPt.GetLocation(), 50, 24, FColor::Yellow, false, 10);
				//DrawDebugSphere(Iter.Actor->GetWorld(), SecondPt.GetLocation(), 50, 24, FColor::Yellow, false, 10);

				auto MinLen = FMath::Min(TempLen1, TempLen2);
				if (MinLen < Len)
				{
					NearestTrackPtr = TempTrackPtr;
					Len = MinLen;
					LinkPos = TempLen1 < TempLen2 ? ETrackVehiclePosState::kFront : ETrackVehiclePosState::kBack;
				}
			}
			else
			{
				NearestTrackPtr = TempTrackPtr;

				FTransform FirstPt = TempTrackPtr->GetLinkPos(ETrackVehiclePosState::kFront);
				FTransform SecondPt = TempTrackPtr->GetLinkPos(ETrackVehiclePosState::kBack);

				auto TempLen1 = FMath::PointDistToLine(FirstPt.GetLocation(), Dir, StopPt);
				auto TempLen2 = FMath::PointDistToLine(SecondPt.GetLocation(), Dir, StopPt);

				//DrawDebugSphere(Iter.Actor->GetWorld(), FirstPt.GetLocation(), 50, 24, FColor::Yellow, false, 10);
				//DrawDebugSphere(Iter.Actor->GetWorld(), SecondPt.GetLocation(), 50, 24, FColor::Yellow, false, 10);

				auto MinLen = FMath::Min(TempLen1, TempLen2);
				Len = MinLen;
				LinkPos = TempLen1 < TempLen2 ? ETrackVehiclePosState::kFront : ETrackVehiclePosState::kBack;
			}
		}

		if (NearestTrackPtr)
		{
			if (NearestTrackPtr->GetCanLinkTrack(LinkPos))
			{
				LinkTrackPtr = NearestTrackPtr;

				FTransform Transform = NearestTrackPtr->GetLinkPos(LinkPos);
				FTransform SecondTransform;

				if (LinkPos == ETrackVehiclePosState::kFront)
				{
					SecondTransform.SetLocation(Transform.GetLocation() - (Transform.GetScale3D().GetSafeNormal() * TrackPtr->GetTrackLen()));
					Transform.SetScale3D(-Transform.GetScale3D());
				}
				else
				{
					SecondTransform.SetLocation(Transform.GetLocation() + (Transform.GetScale3D().GetSafeNormal() * TrackPtr->GetTrackLen()));
				}

				TrackPtr->UpdateSpline(Transform, SecondTransform, true);

				return true;
			}
		}

		LinkTrackPtr = nullptr;

		return false;
	}

	bool FActionTrackPlace::PrevFindLine(const FHitResult& OutHit, const FVector& StartPt, const FVector& StopPt)
	{
		FTransform Transform;
		FTransform SecondTransform;

		Transform.SetLocation(OutHit.ImpactPoint);
		auto Dir = StopPt - StartPt;
		Dir.Normalize();
		Dir.Z = 0;
		SecondTransform.SetLocation(OutHit.ImpactPoint + (Dir * TrackPtr->GetTrackLen()));

		TrackPtr->UpdateSpline(Transform, SecondTransform, false);

		return true;
	}

	void FActionTrackPlace::OnPlaceItemPrev()
	{
		bool bIsCanPlace = false;

		FMinimalViewInfo DesiredView;

		GetOwnerActor<AHumanCharacter>()->GetCameraComp()->GetCameraView(0, DesiredView);

		auto StartPt = DesiredView.Location;
		auto StopPt = DesiredView.Location + (DesiredView.Rotation.Vector() * 1000);

		//	DrawDebugLine(PlayerCharacterPtr->GetWorld(), StartPt, StopPt, FColor::Red, false, 1);

		auto OnwerActorPtr = GetOwnerActor<AHumanCharacter>();

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(TrackPtr);
		ActorsToIgnore.Add(OnwerActorPtr);

		TArray<TEnumAsByte<EObjectTypeQuery>>ObjectTypes;

		FHitResult OutHit;

		if (CurPtIndex == 0)
		{
			TArray<FHitResult> OutHits;

			// ������ж�������Trace���������
			if (UKismetSystemLibrary::SphereTraceMultiForObjects(
				OnwerActorPtr, StartPt, StopPt, FindArea,
				ObjectTypes, false, ActorsToIgnore,
				EDrawDebugTrace::ForDuration, OutHits, true, FLinearColor::Red, FLinearColor::Green, 0
			))
			{
				if (PrevFindSphere(OutHits, StartPt, StopPt))
				{
					bIsCanPlace = true;
					return;
				}
			}

			// ������ж�δ������Trace���������
			if (UKismetSystemLibrary::LineTraceSingleForObjects(
				OnwerActorPtr, StartPt, StopPt,
				ObjectTypes, false, ActorsToIgnore,
				EDrawDebugTrace::ForDuration, OutHit, true, FLinearColor::Red, FLinearColor::Green, 0
			))
			{
				if (PrevFindLine(OutHit, StartPt, StopPt))
				{
					bIsCanPlace = true;
					return;
				}
			}
		}
		else
		{
			if (UKismetSystemLibrary::LineTraceSingleForObjects(
				OnwerActorPtr, StartPt, StopPt,
				ObjectTypes, false, ActorsToIgnore,
				EDrawDebugTrace::ForDuration, OutHit, true, FLinearColor::Red, FLinearColor::Green, 0
			))
			{
				auto Previous = GetPreviousPt();
				auto LinkPt = Previous.GetLocation();

				auto Dir = (OutHit.ImpactPoint - Previous.GetLocation()).GetSafeNormal();
				auto Angle = FMath::Acos(FVector::DotProduct(Dir, Previous.GetScale3D().GetSafeNormal()));

				//DrawDebugLine(PlayerCharacterPtr->GetWorld(), LinkPt, LinkPt + (Dir * 200), FColor::Blue, false, 1);
				//DrawDebugLine(PlayerCharacterPtr->GetWorld(), LinkPt, LinkPt + (Previous.GetScale3D().GetSafeNormal() * 200), FColor::Orange, false, 1);

				if (Angle < FMath::DegreesToRadians(30))
				{
					FTransform SecondTransform;

					SecondTransform.SetLocation(LinkPt + (Dir * TrackPtr->GetTrackLen()));

					TrackPtr->UpdateSpline(Previous, SecondTransform, true);

					bIsCanPlace = true;
					return;
				}
			}
		}
		bIsCanPlace = false;
	}

	FTransform FActionTrackPlace::GetPreviousPt()
	{
		if (LinkTrackPtr)
		{
			return LinkTrackPtr->GetLinkPos(LinkPos);
		}

		if (TrackPtr)
		{
			return TrackPtr->GetLinkPos(ETrackVehiclePosState::kFront);
		}

		return FTransform::Identity;
	}
}