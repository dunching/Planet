
#include "NavgationSubSysetem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>
#include <NavMesh/NavMeshBoundsVolume.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>

#include <AIController.h>
#include <NavigationPath.h>
#include "NavigationSystem.h"
#include "Tools.h"

namespace NavgationSubSysetm
{
	const FName Player = TEXT("Player");
}

UNavgationSubSystem* UNavgationSubSystem::GetInstance()
{
	return Cast<UNavgationSubSystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(GetWorldImp(), UNavgationSubSystem::StaticClass()));
}

void UNavgationSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UNavgationSubSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentInterval += DeltaTime;
	if (CurrentInterval >= Interval)
	{
		CurrentInterval = 0.f;

		CheckNeedRebuildNavData();
	}
}

void UNavgationSubSystem::CheckNeedRebuildNavData()
{
	TArray<AActor*>ResultAry;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ANavMeshBoundsVolume::StaticClass(), NavgationSubSysetm::Player, ResultAry);

	auto CharacterPtr = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!CharacterPtr)
	{
		return;
	}

	for (auto Iter : ResultAry)
	{
		auto NavMeshBoundsVolumePtr = Cast<ANavMeshBoundsVolume>(Iter);
		if (!NavMeshBoundsVolumePtr)
		{
			continue;
		}

		auto Pt1 = CharacterPtr->GetActorLocation();
		auto Pt2 = NavMeshBoundsVolumePtr->GetActorLocation();

		if (
			!PreviouFlyingNavigationDataPtr ||
	//		!(PreviouFlyingNavigationDataPtr && PreviouFlyingNavigationDataPtr->IsNavigationDataBuilt()) ||
			(FVector::Distance(Pt1, Pt2) > 500)
			)
		{
	//		RebuildNavData(NavMeshBoundsVolumePtr, CharacterPtr, Pt1);
			break;
		}
	}
}

void UNavgationSubSystem::RebuildNavData(
	ANavMeshBoundsVolume* NavMeshBoundsVolumePtr,
	ACharacter* CharacterPtr,
	const FVector& TargetPt
)
{
	NavMeshBoundsVolumePtr->SetActorLocation(TargetPt);

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
 		NavSys->OnNavigationBoundsUpdated(NavMeshBoundsVolumePtr);
 		NavSys->Build();
	}
}

TStatId UNavgationSubSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UNavgationSubSystem, STATGROUP_Tickables);
}

void UNavgationSubSystem::FlyingNavGenerationFinished()
{
}
