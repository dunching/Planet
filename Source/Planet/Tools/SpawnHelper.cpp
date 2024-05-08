
#include "SpawnHelper.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

#include "CacheAssetManager.h"
#include "ArticleBase.h"
#include "CollisionDataStruct.h"

ASpawnHelper::ASpawnHelper(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SphereComponentPtr = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponentPtr->SetupAttachment(RootComponent);
}

void ASpawnHelper::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		UpdateGravityCenterLocationTimer, this, &ASpawnHelper::SpanActor, Intervals, true
	);
}

void ASpawnHelper::SpanActor()
{
	if (SpawnActorAry.Num() == 0)
	{
		return;
	}

	const auto Radius = SphereComponentPtr->GetScaledSphereRadius();
	auto X = FMath::RandRange(-Radius, Radius);
	auto Y = FMath::RandRange(-Radius, Radius);
	auto Z = FMath::RandRange(-Radius, Radius);

	FVector Vec(X, Y, Z);

	if (Vec.IsNearlyZero())
	{
		return;
	}

	Vec.Normalize();
	
	auto StartPt = Vec * Radius;
	auto StopPt = FVector::ZeroVector;

	FHitResult Result;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(VoxelWorld);

	FCollisionQueryParams Params;
	Params.bTraceComplex = false;

	DrawDebugLine(GetWorld(), StartPt, StopPt, FColor::Green, false, 10);
	if (GetWorld()->LineTraceSingleByObjectType(
		Result,
		StartPt,
		StopPt,
		ObjectQueryParams,
		Params)
		)
	{
		DrawDebugSphere(GetWorld(), Result.ImpactPoint, 20, 10, FColor::Red, false, 10);

		int32 Index = SpawnActorAry.Num();
		int32 CurrentIndex = 0;
		for (auto Iter : SpawnActorAry)
		{
			CurrentIndex++;
			if (CurrentIndex < Index)
			{
				continue;
			}

			FTransform Transform(FRotationMatrix::MakeFromZ(Vec).ToQuat(), Result.ImpactPoint - (Vec * Offset));

			TArray<AActor*>ResultAry;
			UGameplayStatics::GetAllActorsOfClass(
				this, 
				ARawMaterialBase::StaticClass(),
				ResultAry
			);

			if (ResultAry.Num() < Iter.Value)
			{
				GetWorld()->SpawnActor<ARawMaterialBase>(
					Iter.Key,
					Transform
					);
			}

			break;
		}
	}
}

