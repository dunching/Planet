
#include "GeneratorColony.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"

#include "Component/PlanetChildActorComponent.h"
#include "BuildingArea.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"
#include "NPCComponent.h"

AGeneratorColony::AGeneratorColony(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponentPtr->SetupAttachment(RootComponent);
	SplineComponentPtr->SetClosedLoop(true);

	FormationComponentPtr = CreateDefaultSubobject<UFormationComponent>(TEXT("FormationComponent"));
	FormationComponentPtr->SetupAttachment(RootComponent);

	bReplicates = true;
	SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 60;
}

void AGeneratorColony::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		TArray<AActor*> OutActors;
		GetAttachedActors(OutActors);

		const auto ChildAry = FormationComponentPtr->GetAttachChildren();

		if (ChildAry.Num() >= OutActors.Num())
		{
			for (int32 Index = 0; Index < OutActors.Num(); Index++)
			{
				auto CharacaterPtr = Cast<AHumanCharacter>(OutActors[Index]);
				if (!CharacaterPtr)
				{
					continue;
				}
				auto ControllerPtr = CharacaterPtr->GetController<AHumanAIController>();
				if (!ControllerPtr)
				{
					continue;
				}

				ControllerPtr->PathFollowComponentPtr = ChildAry[Index];
			}
		}
	}
#endif
}

void AGeneratorColony::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		CurrentLength = CurrentLength + (ForwardSpeed * DeltaSeconds);
		const int32 Length = SplineComponentPtr->GetSplineLength();
		if (CurrentLength > Length)
		{
			CurrentLength = CurrentLength % Length;
		}
		const auto Transform = SplineComponentPtr->GetTransformAtDistanceAlongSpline(CurrentLength, ESplineCoordinateSpace::Local);
		FormationComponentPtr->SetRelativeTransform(Transform);
	}
#endif
}

UFormationComponent::UFormationComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}
