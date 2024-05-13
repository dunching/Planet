
#include "NavgationSubSysetem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>
#include <NavMesh/NavMeshBoundsVolume.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>
#include <AIController.h>
#include <NavigationPath.h>

#include "FlyingNavigationData.h"
#include "FlyingNavFunctionLibrary.h"

namespace NavgationSubSysetm
{
	const FName Player = TEXT("Player");
}

UNavgationSubSystem* UNavgationSubSystem::GetInstance()
{
	return Cast<UNavgationSubSystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(GetWorldImp(), UNavgationSubSystem::StaticClass()));
}

void UNavgationSubSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentInterval += DeltaTime;
	if (CurrentInterval >= Interval)
	{
		CurrentInterval = 0.f;

		TArray<AActor*>ResultAry;
		UGameplayStatics::GetAllActorsOfClassWithTag(this, ANavMeshBoundsVolume::StaticClass(), NavgationSubSysetm::Player, ResultAry);

		auto ChracterPtr = UGameplayStatics::GetPlayerCharacter(this, 0);

		for (auto Iter : ResultAry)
		{
			auto NavMeshBoundsVolumePtr = Cast<ANavMeshBoundsVolume>(Iter);
			if (!NavMeshBoundsVolumePtr)
			{
				continue;
			}

			auto Pt1 = ChracterPtr->GetActorLocation();
			auto Pt2 = NavMeshBoundsVolumePtr->GetActorLocation();

			if (FVector::Distance(Pt1, Pt2) > 500)
			{
				NavMeshBoundsVolumePtr->SetActorLocation(Pt1);

				UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
				if (NavSys)
				{
					NavSys->OnNavigationBoundsUpdated(NavMeshBoundsVolumePtr);
				}

				if (!PreviouFlyingNavigationDataPtr)
				{
					PreviouFlyingNavigationDataPtr = UFlyingNavFunctionLibrary::GetFlyingNavigationData(ChracterPtr);
					PreviouFlyingNavigationDataPtr->OnFlyingNavGenerationFinished.AddDynamic(this, &ThisClass::FlyingNavGenerationFinished);
				}
				if (PreviouFlyingNavigationDataPtr)
				{


					PreviouFlyingNavigationDataPtr->StopRebuild();
					PreviouFlyingNavigationDataPtr->RebuildNavigationData();
				}
				break;
			}
		}
	}
}

TStatId UNavgationSubSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UNavgationSubSystem, STATGROUP_Tickables);
}

void UNavgationSubSystem::FlyingNavGenerationFinished()
{
}
