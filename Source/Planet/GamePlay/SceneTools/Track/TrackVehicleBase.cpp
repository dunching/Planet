
#include "TrackVehicleBase.h"

#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Components/SplineComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"

#include "ToolsLibrary.h"
#include "CollisionDataStruct.h"
#include "TrackBase.h"
#include "Pawn/HoldingItemsComponent.h"

ATrackVehicleBase::ATrackVehicleBase(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
	RootCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootCompPtr->SetMobility(EComponentMobility::Movable);
	RootComponent = RootCompPtr;

	SceneCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SceneCompPtr->SetMobility(EComponentMobility::Movable);
	SceneCompPtr->SetupAttachment(RootCompPtr);

    WheelMeshFLPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshFL"));
	WheelMeshFLPtr->SetupAttachment(SceneCompPtr);
	WheelMeshFLPtr->SetMobility(EComponentMobility::Movable);
	WheelMeshFLPtr->SetCollisionProfileName(VehicleItem);

    WheelMeshFRPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshFR"));
    WheelMeshFRPtr->SetupAttachment(SceneCompPtr);
	WheelMeshFRPtr->SetMobility(EComponentMobility::Movable);
	WheelMeshFRPtr->SetCollisionProfileName(VehicleItem);

    WheelMeshBFPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshBL"));
    WheelMeshBFPtr->SetupAttachment(SceneCompPtr);
	WheelMeshBFPtr->SetMobility(EComponentMobility::Movable);
	WheelMeshBFPtr->SetCollisionProfileName(VehicleItem);

    WheelMeshBRPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshBR"));
    WheelMeshBRPtr->SetupAttachment(SceneCompPtr);
	WheelMeshBRPtr->SetMobility(EComponentMobility::Movable);
	WheelMeshBRPtr->SetCollisionProfileName(VehicleItem);

	MeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OtherMesh"));
	MeshPtr->SetupAttachment(SceneCompPtr);
	MeshPtr->SetMobility(EComponentMobility::Movable);
	MeshPtr->SetCollisionProfileName(VehicleItem);

	ItemPropertyComponentPtr = CreateDefaultSubobject<UTrackVehicleItemPropertyComponent>(UTrackVehicleItemPropertyComponent::ComponentName);
	HoldItemComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
}

void ATrackVehicleBase::BeginPlay()
{
    Super::BeginPlay();
}

void ATrackVehicleBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ATrackVehicleBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

    switch (TrackVehicleDir)
    {
    case ETrackVehicleDir::kFront:
	{
		if (CurSpeed < AbsSpeed)
		{
			CurSpeed = CurSpeed + (DeltaSeconds * Acceleration);
		}
    }
    break;
    case ETrackVehicleDir::kBack:
	{
		if (FMath::Abs(CurSpeed) < AbsSpeed)
		{
			CurSpeed = CurSpeed - (DeltaSeconds * Acceleration);
		}
    }
        break;
    case ETrackVehicleDir::kStop:
    {
        if (CurSpeed > 0)
		{
			CurSpeed = CurSpeed - (DeltaSeconds * Acceleration);
        }
    }
		break;
	default:
		break;
    }

	if (CurSpeed > 0)
	{
        WheelRot.Pitch = WheelRot.Pitch + (WheelRot.Pitch * CurSpeed * DeltaSeconds);

        WheelRot.Pitch = WheelRot.Pitch / 360.f;

		WheelMeshFLPtr->SetRelativeRotation(WheelRot);
        WheelMeshFRPtr->SetRelativeRotation(WheelRot);
        WheelMeshBFPtr->SetRelativeRotation(WheelRot);
        WheelMeshBRPtr->SetRelativeRotation(WheelRot);
	}
}

void ATrackVehicleBase::UpdatePos()
{
	if (AttachedTrackTrackPtr)
	{
		auto Len = AttachedTrackTrackPtr->GetTrackLen();

		switch (TrackVehicleDir)
		{
		case ETrackVehicleDir::kFront:
		{
			RunLen -= Speed;
			if (RunLen < 20)
			{
				auto NewTrackPtr = AttachedTrackTrackPtr->GetPrevTrack();
				if (NewTrackPtr)
				{
					RunLen += Len;
					SetAttchedTrack(NewTrackPtr);
				}
				else
				{
					RunLen = 20;
					TrackVehicleDir = ETrackVehicleDir::kStop;
					GetWorld()->GetTimerManager().ClearTimer(FixPosTimer);
				}
			}

			auto Transform = AttachedTrackTrackPtr->GetSplinePtByLen(RunLen);

			SetActorLocation(Transform.GetLocation());
			SetActorRotation(Transform.GetRotation());
		}
		break;
		case ETrackVehicleDir::kBack:
		{
			RunLen += Speed;
			if (RunLen > (Len - 20))
			{
				auto NewTrackPtr = AttachedTrackTrackPtr->GetNextTrack();
				if (NewTrackPtr)
				{
					RunLen -= Len;
					SetAttchedTrack(NewTrackPtr);
				}
				else
				{
					RunLen = Len - 20;
					TrackVehicleDir = ETrackVehicleDir::kStop;
					GetWorld()->GetTimerManager().ClearTimer(FixPosTimer);
				}
			}

			auto Transform = AttachedTrackTrackPtr->GetSplinePtByLen(RunLen);

			SetActorLocation(Transform.GetLocation());
			SetActorRotation(Transform.GetRotation());
		}
		break;
		}
	}
}

void ATrackVehicleBase::SetState(ETrackVehicleDir NewTrackVehicleDir)
{
    TrackVehicleDir = NewTrackVehicleDir;
}

void ATrackVehicleBase::SetAttchedTrack(ATrackBase* NewAttachedTrack)
{
	AttachedTrackTrackPtr = NewAttachedTrack;
}

ATrackBase* ATrackVehicleBase::GetAttchedTrack()
{
	return AttachedTrackTrackPtr;
}

void ATrackVehicleBase::SetPrevMaterial(UMaterialInterface* PrevMaterialPtr)
{
}

void ATrackVehicleBase::SetPlaced()
{
}

void ATrackVehicleBase::DriveToTheWay(ETrackVehicleDir NewTrackVehicleDir)
{
	if (AttachedTrackTrackPtr)
	{
		TrackVehicleDir = NewTrackVehicleDir;

		GetWorld()->GetTimerManager().SetTimer(
			FixPosTimer, this, &ATrackVehicleBase::UpdatePos, 1.f / static_cast<float>(UpdateRadio), true
		);
	}
}

void ATrackVehicleBase::UpdateCurrentRunLen(float NewRunLen)
{
	RunLen = NewRunLen;
}

UTrackVehicleItemPropertyComponent::UTrackVehicleItemPropertyComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}
