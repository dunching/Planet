
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

#include <CacheAssetManager.h>
#include "ArticleBase.h"
#include "ArticleStone.h"
#include "ArticleTree.h"
#include "GameMode/PlanetGameMode.h"
#include "CollisionDataStruct.h"
#include <HumanCharacter.h>
#include <Character/GravityMovementComponent.h>

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

		UGameplayStatics::GetAllActorsOfClass(GGameInstancePtr, ARawMaterialBase::StaticClass(), ResultAry);

		const auto Extent = BoxCompPtr->GetScaledBoxExtent();
		const auto Transform = GetActorTransform();

		FBox Box(-Extent, Extent);

		const auto TargetPos = Transform.TransformPosition(FMath::RandPointInBox(Box));

		TArray<AActor*> ActorsToIgnore;

		TArray<TEnumAsByte<EObjectTypeQuery>>ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(VoxelWorld));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(Building));

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