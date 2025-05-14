#include "SplineMesh.h"

#include "GameOptions.h"
#include "Components/AudioComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Concepts/Iterable.h"
#include "Kismet/GameplayStatics.h"
#include "Trace/Detail/Transport.h"
#include "Widgets/Text/STextScroller.h"

#include "HumanCharacter_Player.h"

inline ASplineMesh::ASplineMesh(
	const FObjectInitializer& ObjectInitializer
	)
{
	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponentPtr->SetClosedLoop(true);

	RootComponent = SplineComponentPtr;
}

void ASplineMesh::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);

	SplineMeshComponentsAry.Empty();

	auto Lambda = [this](
		USplineComponent* SplineComponentPtr,
		float Start,
		float End
		)
	{
		const auto Transform1 = SplineComponentPtr->GetTransformAtDistanceAlongSpline(
			 Start,
			 ESplineCoordinateSpace::World,
			 true
			);
		const auto Tangent1 = SplineComponentPtr->GetTangentAtDistanceAlongSpline(Start, ESplineCoordinateSpace::World);

		const auto Transform2 = SplineComponentPtr->GetTransformAtDistanceAlongSpline(
			 End,
			 ESplineCoordinateSpace::World,
			 true
			);
		const auto Tangent2 = SplineComponentPtr->GetTangentAtDistanceAlongSpline(End, ESplineCoordinateSpace::World);

		auto SplineMeshPtr = Cast<USplineMeshComponent>(
		                                                AddComponentByClass(
		                                                                    USplineMeshComponent::StaticClass(),
		                                                                    true,
		                                                                    FTransform::Identity,
		                                                                    false
		                                                                   )
		                                               );

		SplineMeshComponentsAry.Add(SplineMeshPtr);

		SplineMeshPtr->SetStaticMesh(StaticMeshRef.LoadSynchronous());
		SplineMeshPtr->SetForwardAxis(ESplineMeshAxis::Y);
		SplineMeshPtr->SetMaterial(0, MaterialRef.LoadSynchronous());

		SplineMeshPtr->SetCollisionProfileName(CollisionProfileName);

		SplineMeshPtr->SetStartPosition(Transform1.GetLocation());
		SplineMeshPtr->SetStartTangent(Tangent1);
		SplineMeshPtr->SetStartScale(FVector2D(1, ScaleZ));

		SplineMeshPtr->SetEndPosition(Transform2.GetLocation());
		SplineMeshPtr->SetEndTangent(Tangent2);
		SplineMeshPtr->SetEndScale(FVector2D(1, ScaleZ));
	};

	const auto Len = SplineComponentPtr->GetSplineLength();
	float Start = 0;
	float End = Start + IntervalLength;
	for (; End < Len;)
	{
		Lambda(SplineComponentPtr, Start, End);
		Start += IntervalLength;
		End = Start + IntervalLength;
	}

	Lambda(SplineComponentPtr, Start, 0);
}

void ASplineMesh::Tick(
	float DeltaSeconds
	)
{
	Super::Tick(DeltaSeconds);
}

ARegionAirWall::ARegionAirWall(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
{
	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponentPtr->SetClosedLoop(true);

	RootComponent = SplineComponentPtr;
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;
}

void ARegionAirWall::BeginPlay()
{
	Super::BeginPlay();
}

void ARegionAirWall::Tick(
	float DeltaSeconds
	)
{
	Super::Tick(DeltaSeconds);

	// SplineComponentPtr->FindLocationClosestToWorldLocation();
}

void ARegionAirWall::OnEntryRegion(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	if (OtherActor->IsA(AHumanCharacter_Player::StaticClass()))
	{

	}
}

void ARegionAirWall::OnLeaveRegion(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
	)
{
	if (OtherActor->IsA(AHumanCharacter_Player::StaticClass()))
	{
		if (AudioComponentPtr)
		{
			AudioComponentPtr->Stop();
		}
		AudioComponentPtr = nullptr;
	}
}
