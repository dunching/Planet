
#include "GeneratorColony_ByInvoke.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"

#include "PlanetChildActorComponent.h"
#include "BuildingArea.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"
#include "AIComponent.h"
#include "GroupManagger_NPC.h"
#include "HumanCharacter_AI.h"

UFormationComponent::UFormationComponent(const FObjectInitializer& ObjectInitializer):
                                                                                     Super(ObjectInitializer)
{
	// SetIsReplicatedByDefault(true);
}

AGeneratorColonyWithPath::AGeneratorColonyWithPath(const FObjectInitializer& ObjectInitializer) :
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

void AGeneratorColonyWithPath::BeginPlay()
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

			}
		}
	}
#endif
}

void AGeneratorColonyWithPath::Tick(float DeltaSeconds)
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

void AGeneratorNPCs_Patrol::SpawnGeneratorActor()
{
	Super::SpawnGeneratorActor();
}

void AGeneratorNPCs_Patrol::CustomizerGroupManagger(
	AGroupManagger_NPC* TargetActorPtr
)
{
	Super::CustomizerGroupManagger(TargetActorPtr);
	
	TargetActorPtr->GeneratorNPCs_PatrolPtr = this;
}

AGeneratorColony_ByInvoke::AGeneratorColony_ByInvoke(
	const FObjectInitializer& ObjectInitializer
	):
		Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// bReplicates = true;
	// SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 60;
}

void AGeneratorColony_ByInvoke::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneratorColony_ByInvoke::SpawnGeneratorActor()
{
	Super::SpawnGeneratorActor();
}

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
	
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		SpawnGeneratorActor();
	}
#endif
}

bool AGeneratorNPCs_Patrol::CheckIsFarawayOriginal(ACharacterBase* TargetCharacterPtr) const
{
	if (TargetCharacterPtr)
	{
		const auto CharacterPt = TargetCharacterPtr->GetActorLocation();
		const auto Pt = SplineComponentPtr->FindLocationClosestToWorldLocation(
			CharacterPt, ESplineCoordinateSpace::World
		);
		const auto Distance = FVector::Distance(Pt, CharacterPt);

		return Distance > MaxDistance;
	}
	else
	{
		return false;
	}
}

