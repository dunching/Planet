// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "TrackBase.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Components/SplineComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "CollisionDataStruct.h"
#include "Kismet/GameplayStatics.h"
#include "TrackVehicleBase.h"

ATrackBase::ATrackBase(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
	SceneCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneCompPtr;

	SplineMesh1Ptr = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SplineComp1"));
	SplineMesh1Ptr->SetupAttachment(RootComponent);
	SplineMesh1Ptr->SetMobility(EComponentMobility::Movable);
    SplineMesh1Ptr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    {
        auto StaticMeshPtr = LoadObject<UStaticMesh>(this, TEXT("StaticMesh'/Game/Planet/GamePlay/SceneObj/Track/Track1.Track1'"));
		SplineMesh1Ptr->SetStaticMesh(StaticMeshPtr);
    }

	SplineMesh2Ptr = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SplineComp2"));
	SplineMesh2Ptr->SetupAttachment(RootComponent);
	SplineMesh2Ptr->SetMobility(EComponentMobility::Movable);
    SplineMesh2Ptr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	{
		auto StaticMeshPtr = LoadObject<UStaticMesh>(this, TEXT("StaticMesh'/Game/Planet/GamePlay/SceneObj/Track/Track2.Track2'"));
        SplineMesh2Ptr->SetStaticMesh(StaticMeshPtr);
	}

    SplineMesh3Ptr = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SplineComp3"));
	SplineMesh3Ptr->SetupAttachment(RootComponent);
	SplineMesh3Ptr->SetMobility(EComponentMobility::Movable);
    SplineMesh3Ptr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	{
		auto StaticMeshPtr = LoadObject<UStaticMesh>(this, TEXT("StaticMesh'/Game/Planet/GamePlay/SceneObj/Track/Track3.Track3'"));
		SplineMesh3Ptr->SetStaticMesh(StaticMeshPtr);
	}

	SplinePtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
    SplinePtr->SetupAttachment(RootComponent);
}

EPickType ATrackBase::GetPickType() const
{
    return PickType;
}

void ATrackBase::BeginPlay()
{
    Super::BeginPlay();

    PickType = SetDefaultPickType();

    HP = GetDefaultHP();
}

int32 ATrackBase::GetCurHP()const
{
    return HP;
}

bool ATrackBase::Attack(int32 Val)
{
    HP -= Val;

    if (HP <= 0)
    {
        Destroy();

        return true;
    }
    return false;
}

void ATrackBase::Interaction(ACharacterBase* CharacterPtr)
{

}

void ATrackBase::UpdateSpline(const FTransform& StartTransForm, const FTransform& StopTransForm, bool bIsUseCutomTan)
{
	SplinePtr->ClearSplinePoints();
	SplinePtr->AddSplinePoint(StartTransForm.GetLocation(), ESplineCoordinateSpace::World);
	SplinePtr->AddSplinePoint(StopTransForm.GetLocation(), ESplineCoordinateSpace::World);
	SplinePtr->SetSplinePointType(0, ESplinePointType::Curve);
	SplinePtr->SetSplinePointType(1, ESplinePointType::Curve);
    if (bIsUseCutomTan)
	{
		// ֻʹ�õ�һ���������
		SplinePtr->SetTangentAtSplinePoint(0, StartTransForm.GetScale3D(), ESplineCoordinateSpace::World);

		auto Dir = (StopTransForm.GetLocation() - StartTransForm.GetLocation()).GetSafeNormal();
		auto Angle = FMath::Acos(FVector::DotProduct(Dir, StartTransForm.GetScale3D().GetSafeNormal()));

		if (FVector::CrossProduct(Dir, StartTransForm.GetScale3D()).Z > 0)
		{
			Angle = -Angle;
		}

		auto SecondTan = Dir.RotateAngleAxis(FMath::RadiansToDegrees(Angle), FVector::UpVector);

		SplinePtr->SetTangentAtSplinePoint(1, SecondTan * GetTrackLen(), ESplineCoordinateSpace::World);
    }

	DrawDebugSphere(GetWorld(), SplinePtr->GetWorldLocationAtSplinePoint(0), 12, 12, FColor::Yellow);
	DrawDebugSphere(GetWorld(), SplinePtr->GetWorldLocationAtSplinePoint(1), 12, 12, FColor::Yellow);

	SplineMesh1Ptr->SetStartPosition(SplinePtr->GetWorldLocationAtSplinePoint(0));
	SplineMesh1Ptr->SetStartTangent(SplinePtr->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World));

	SplineMesh1Ptr->SetEndPosition(SplinePtr->GetWorldLocationAtSplinePoint(1));
	SplineMesh1Ptr->SetEndTangent(SplinePtr->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World));

	SplineMesh2Ptr->SetStartPosition(SplinePtr->GetWorldLocationAtSplinePoint(0));
	SplineMesh2Ptr->SetStartTangent(SplinePtr->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World));

	SplineMesh2Ptr->SetEndPosition(SplinePtr->GetWorldLocationAtSplinePoint(1));
	SplineMesh2Ptr->SetEndTangent(SplinePtr->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World));

	SplineMesh3Ptr->SetStartPosition(SplinePtr->GetWorldLocationAtSplinePoint(0));
	SplineMesh3Ptr->SetStartTangent(SplinePtr->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World));

	SplineMesh3Ptr->SetEndPosition(SplinePtr->GetWorldLocationAtSplinePoint(1));
	SplineMesh3Ptr->SetEndTangent(SplinePtr->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World));
}

bool ATrackBase::GetCanLinkTrack(ETrackVehiclePosState Pos) const
{
    if (Pos == ETrackVehiclePosState::kFront)
    {
        return PreviousTrackPtr == nullptr;
    }
    else
	{
		return NextTrackPtr == nullptr;
    }
}

FTransform ATrackBase::GetLinkPos(ETrackVehiclePosState Pos)
{
    FTransform Transform;

	if (Pos == ETrackVehiclePosState::kFront)
	{
		Transform.SetLocation(SplineMesh1Ptr->GetStartPosition());
		Transform.SetScale3D(SplineMesh1Ptr->GetStartTangent());
    }
    else
	{
		Transform.SetLocation(SplineMesh1Ptr->GetEndPosition());
		Transform.SetScale3D(SplineMesh1Ptr->GetEndTangent());
    }

    return Transform;
}

FBearerState ATrackBase::CheckTrackBearerState() const
{
	TArray<AActor*>Result;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrackVehicleBase::StaticClass(), Result);

	for (auto VehicleTrackIter : Result)
	{
		auto VehicleTrackPtr = Cast<ATrackVehicleBase>(VehicleTrackIter);
		if (VehicleTrackPtr)
		{
			auto VehicleAttchedTrackPtr = VehicleTrackPtr->GetAttchedTrack();
			if (VehicleAttchedTrackPtr == this)
			{
				return FBearerState::kHaveVehicleTrack;
			}
			auto TempTrackPtr = PreviousTrackPtr;
			for (; TempTrackPtr;)
			{
				if (VehicleAttchedTrackPtr == TempTrackPtr)
				{
					return FBearerState::kHaveVehicleTrack;
				}
				TempTrackPtr = TempTrackPtr->GetPrevTrack();
			}
			 TempTrackPtr = NextTrackPtr;
			for (; TempTrackPtr;)
			{
				if (VehicleAttchedTrackPtr == TempTrackPtr)
				{
					return FBearerState::kHaveVehicleTrack;
				}
				TempTrackPtr = TempTrackPtr->GetNextTrack();
			}
		}
	}

	return FBearerState::kNoVehicleTrack;
}

void ATrackBase::RemoveTrack(ATrackBase* TartgetTrackPtr)
{
    if (TartgetTrackPtr)
    {
        if (TartgetTrackPtr == PreviousTrackPtr)
		{
            PreviousTrackPtr = nullptr;
        }
        else if (TartgetTrackPtr == NextTrackPtr)
        {
            NextTrackPtr = nullptr;
        }
    }
}

FTransform ATrackBase::GetSplinePtByLen(float Len) const
{
	auto Pt = SplinePtr->GetLocationAtDistanceAlongSpline(Len, ESplineCoordinateSpace::World);
	auto Rot= SplinePtr->GetTangentAtDistanceAlongSpline(Len, ESplineCoordinateSpace::World);

	return FTransform(Rot.Rotation(), Pt);
}

float ATrackBase::GetTrackLen() const
{
    return TrackLen;
}

float ATrackBase::GetTrackTotalLen()const
{
	float Len = 0.f;

	Len += SplinePtr->GetSplineLength();

	auto TempTrackPtr = PreviousTrackPtr;
	for (; TempTrackPtr;)
	{
		Len += TempTrackPtr->SplinePtr->GetSplineLength();

		TempTrackPtr = PreviousTrackPtr->GetPrevTrack();
	}

	TempTrackPtr = NextTrackPtr;
	for (; TempTrackPtr;)
	{
		Len += TempTrackPtr->SplinePtr->GetSplineLength();

		TempTrackPtr = PreviousTrackPtr->GetNextTrack();
	}

	return Len;
}

TPair<FTransform, float>  ATrackBase::GetProjectPt(const FVector& TargetPt) const
{
	auto Len = SplinePtr->GetSplineLength();
	auto Pt1 = SplinePtr->GetLocationAtDistanceAlongSpline(TrackLen * 0.2f, ESplineCoordinateSpace::World);
	auto Pt2 = SplinePtr->GetLocationAtDistanceAlongSpline(TrackLen * 0.8f, ESplineCoordinateSpace::World);

	auto Len1 = FVector::Distance(Pt1, TargetPt);
	auto Len2 = FVector::Distance(Pt2, TargetPt);

	TPair<FTransform, float>Result;

	Result.Key = FTransform(
		SplinePtr->GetTangentAtDistanceAlongSpline(TrackLen * 0.2f, ESplineCoordinateSpace::World).Rotation(),
		Pt1
	);


	if (Len1 < Len2)
	{
		Result.Value = TrackLen * 0.2f;
	}
	else
	{
		Result.Value = TrackLen * 0.8f;
	}

	return Result;
}

void ATrackBase::SetLinkTrack(ATrackBase* NewTrackPtr, ETrackVehiclePosState Pos)
{
	switch (Pos)
	{
	case ETrackVehiclePosState::kFront:
	{
		PreviousTrackPtr = NewTrackPtr;
		NewTrackPtr->NextTrackPtr = this;
	}
		break;
	case ETrackVehiclePosState::kBack:
	{
		NextTrackPtr = NewTrackPtr;
		NewTrackPtr->PreviousTrackPtr = this;
	}
		break;
	default:
		break;
	}
}

void ATrackBase::GetCircular(
	const FVector& P1,
	const FVector& P2,
	const FVector& P3,
	FVector& PCenter
)
{
	double a1, b1, c1, d1;
	double a2, b2, c2, d2;
	double a3, b3, c3, d3;

	double x1 = P1.X, y1 = P1.Y, z1 = P1.Z;
	double x2 = P2.X, y2 = P2.Y, z2 = P2.Z;
	double x3 = P3.X, y3 = P3.Y, z3 = P3.Z;

	a1 = (y1 * z2 - y2 * z1 - y1 * z3 + y3 * z1 + y2 * z3 - y3 * z2);
	b1 = -(x1 * z2 - x2 * z1 - x1 * z3 + x3 * z1 + x2 * z3 - x3 * z2);
	c1 = (x1 * y2 - x2 * y1 - x1 * y3 + x3 * y1 + x2 * y3 - x3 * y2);
	d1 = -(x1 * y2 * z3 - x1 * y3 * z2 - x2 * y1 * z3 + x2 * y3 * z1 + x3 * y1 * z2 - x3 * y2 * z1);

	a2 = 2 * (x2 - x1);
	b2 = 2 * (y2 - y1);
	c2 = 2 * (z2 - z1);
	d2 = x1 * x1 + y1 * y1 + z1 * z1 - x2 * x2 - y2 * y2 - z2 * z2;

	a3 = 2 * (x3 - x1);
	b3 = 2 * (y3 - y1);
	c3 = 2 * (z3 - z1);
	d3 = x1 * x1 + y1 * y1 + z1 * z1 - x3 * x3 - y3 * y3 - z3 * z3;

	PCenter.X = -(b1 * c2 * d3 - b1 * c3 * d2 - b2 * c1 * d3 + b2 * c3 * d1 + b3 * c1 * d2 - b3 * c2 * d1)
		/ (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);
	PCenter.Y = (a1 * c2 * d3 - a1 * c3 * d2 - a2 * c1 * d3 + a2 * c3 * d1 + a3 * c1 * d2 - a3 * c2 * d1)
		/ (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);
	PCenter.Z = -(a1 * b2 * d3 - a1 * b3 * d2 - a2 * b1 * d3 + a2 * b3 * d1 + a3 * b1 * d2 - a3 * b2 * d1)
		/ (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);
}

int32 ATrackBase::GetDefaultHP()
{
    return 100;
}

EPickType ATrackBase::SetDefaultPickType()
{
    return EPickType::kNone;
}

UTrackBasePropertyComponent::UTrackBasePropertyComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}
