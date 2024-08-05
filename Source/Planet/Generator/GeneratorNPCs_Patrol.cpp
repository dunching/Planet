
#include "GeneratorNPCs_Patrol.h"

#include <Components/SplineComponent.h>

#include "Component/PlanetChildActorComponent.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"

AGeneratorNPCs_Patrol::AGeneratorNPCs_Patrol(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("USceneComponent"));

	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponentPtr->SetupAttachment(RootComponent);
}

void AGeneratorNPCs_Patrol::BeginPlay()
{
	Super::BeginPlay();

	TArray<UChildActorComponent*>Components;
	GetComponents(UChildActorComponent::StaticClass(), Components);
	for (const auto Iter : Components)
	{
		Iter->CreateChildActor();
	}

	TArray<USplineComponent*>SPlineAry;
	SPlineAry.Add(SplineComponentPtr);
	for (int32 Index = 0; Index < SampleNum; Index++)
	{
		auto SPlinePtr = Cast<USplineComponent>(AddComponentByClass(USplineComponent::StaticClass(), true, FTransform::Identity, false));
		if (SPlinePtr)
		{
			SPlineAry.Add(SPlinePtr);
			SPlinePtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			SPlinePtr->ClearSplinePoints();
			auto PointNum = SplineComponentPtr->GetNumberOfSplinePoints();
			for (int32 PointIndex = 0; PointIndex < PointNum; PointIndex++)
			{
				const auto Point = SplineComponentPtr->GetSplinePointAt(PointIndex, ESplineCoordinateSpace::World);

				FSplinePoint NewPoint;

				NewPoint = Point;

				NewPoint.Position = NewPoint.Position - (NewPoint.Rotation.Quaternion().GetRightVector() * (Index * Offset));

				SPlinePtr->AddPoint(NewPoint, false);
			}
			SPlinePtr->UpdateSpline();
		}
	}

	ForEachComponent<UChildActorComponent>(true, [&](UChildActorComponent* ComPtr) {
		if (ComPtr->GetChildActor()->IsA(AHumanCharacter::StaticClass()))
		{
			auto CharacterPtr = Cast<AHumanCharacter>(ComPtr->GetChildActor());
			if (CharacterPtr)
			{
				auto AIControllerPtr = Cast<AHumanAIController>(CharacterPtr->GetController());
				if (AIControllerPtr)
				{
					AIControllerPtr->PatrolSPlinePtr = SPlineAry[FMath::RandRange(0, SPlineAry.Num() - 1)];
				}
			}
		}
		});
}

