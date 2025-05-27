
#include "ArticleSpawnBox.h"

#include <thread>
#include <functional>

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"
#include "Async/Async.h"
#include <Kismet/KismetMathLibrary.h>

#include "ArticleBase.h"
#include "ArticleStone.h"
#include "ArticleTree.h"
#include "GameMode_Main.h"
#include "CollisionDataStruct.h"
#include <HumanCharacter.h>
#include <Character/GravityMovementComponent.h>

#include "Tools.h"

AArticleSpawnBox::AArticleSpawnBox() :
                                     Super()
{
	BoxCompPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxCompPtr->SetBoxExtent(FVector(500, 500, 10));
	BoxCompPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxCompPtr->SetMobility(EComponentMobility::Static);
	RootComponent = BoxCompPtr;
}

void AArticleSpawnBox::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(SpawnActorTimer, this, &AArticleSpawnBox::SpawnItem, SpawnDensity, true);
}

void AArticleSpawnBox::SpawnItem()
{
	if (bIsReSpawn)
	{
		TArray<AActor*>ResultAry;

		UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), ARawMaterialBase::StaticClass(), ResultAry);

		const auto Extent = BoxCompPtr->GetScaledBoxExtent();
		const auto Transform = GetActorTransform();

		FBox Box(-Extent, Extent);

		const auto TargetPos = Transform.TransformPosition(FMath::RandPointInBox(Box));

		TArray<AActor*> ActorsToIgnore;

		TArray<TEnumAsByte<EObjectTypeQuery>>ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(VoxelWorld_Object));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(Building_Object));

		FHitResult OutHits;

		if (UKismetSystemLibrary::LineTraceSingleForObjects(
			this, TargetPos, TargetPos + (Transform.GetRotation().GetAxisZ() * 10000),
			ObjectTypes, false, ActorsToIgnore,
			EDrawDebugTrace::ForDuration, OutHits, true, FLinearColor::Red, FLinearColor::Green
		))
		{
		}
	}
}