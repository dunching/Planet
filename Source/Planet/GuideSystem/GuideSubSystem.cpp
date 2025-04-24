#include "GuideSubSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Planet_Tools.h"
#include "PlanetWorldSettings.h"
#include "GuideActor.h"
#include "GuideSystemStateTreeComponent.h"
#include "GuideThread.h"

bool UGuideSubSystem::ShouldCreateSubsystem(
	UObject* Outer
) const
{
	if (IsRunningDedicatedServer())
	{
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}

UGuideSubSystem* UGuideSubSystem::GetInstance()
{
	return Cast<UGuideSubSystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(GetWorldImp(), UGuideSubSystem::StaticClass())
	);
}

void UGuideSubSystem::InitializeMainThread()
{
}

void UGuideSubSystem::ActiveMainThread()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());

	ActiveTargetGuideThread(WorldSetting->MainLineGuideClass);
}

void UGuideSubSystem::AddGuideThread_Brand(
	const TSubclassOf<AGuideThread_Branch>& BranchLineGuideClass
)
{
	for (int32 Index = 0; Index < GuideThreadBranchAry.Num(); Index++)
	{
		if (GuideThreadBranchAry[Index] == BranchLineGuideClass)
		{
			return;
		}
	}

	GuideThreadBranchAry.Add(BranchLineGuideClass);
}

void UGuideSubSystem::ActiveBrandGuideThread(
	const TSubclassOf<AGuideThread_Branch>& GuideClass
)
{
	ActiveTargetGuideThread(GuideClass);
}

void UGuideSubSystem::StopActiveBrandGuideThread(
	const TSubclassOf<AGuideThread_Branch>& GuideClass
)
{
	if (CurrentLineGuidePtr)
	{
		if (CurrentLineGuidePtr->IsA(GuideClass))
		{
			GuidePostionSet.Add(CurrentLineGuidePtr->GetPreviousTaskID());
			CurrentLineGuidePtr->Destroy();

			CurrentLineGuidePtr = nullptr;
			PreviouGuideClass = nullptr;
		}
	}
}

void UGuideSubSystem::StartParallelGuideThread(
	const TSubclassOf<AGuideThread_Immediate>& GuideClass
)
{
	for (auto Iter : ActivedGuideThreadsAry)
	{
		if (Iter->IsA(GuideClass))
		{
			return;
		}
	}

	FActorSpawnParameters SpawnParameters;

	auto GuideThreadPtr = GetWorld()->SpawnActor<AGuideThread>(
		GuideClass,
		SpawnParameters
	);

	GuideThreadPtr->GetGuideSystemStateTreeComponent()->StartLogic();

	OnStartGuide.Broadcast(GuideThreadPtr);
	ActivedGuideThreadsAry.Add(GuideThreadPtr);
}

void UGuideSubSystem::StopParallelGuideThread(
	const TSubclassOf<AGuideThread_Immediate>& GuideClass
)
{
	for (int32 Index = 0; Index < ActivedGuideThreadsAry.Num(); Index++)
	{
		if (ActivedGuideThreadsAry[Index]->IsA(GuideClass))
		{
			ActivedGuideThreadsAry[Index]->Destroy();
			ActivedGuideThreadsAry.RemoveAt(Index);
			return;
		}
	}
}

void UGuideSubSystem::GuideThreadEnded(
	AGuideThread* GuideThreadPtr
)
{
	OnStopGuide.Broadcast(GuideThreadPtr);

	if (GuideThreadPtr)
	{
		const auto CurrentGuideID = GuideThreadPtr->GetGuideID();
		const auto CurrentTaskID = GuideThreadPtr->GetCurrentTaskID();

		if (GuidePostionSet.Contains(CurrentGuideID))
		{
			//
			GuidePostionSet.Remove(CurrentGuideID);
		}

		GuideThreadPtr->Destroy();
		GuideThreadPtr = nullptr;
	}
}

TObjectPtr<AGuideThread> UGuideSubSystem::GetCurrentGuideThread() const
{
	return CurrentLineGuidePtr;
}

FOnStartGuide& UGuideSubSystem::GetOnStartGuide()
{
	return OnStartGuide;
}

FOnStopGuide& UGuideSubSystem::GetOnStopGuide()
{
	return OnStopGuide;
}

void UGuideSubSystem::ActiveTargetGuideThread(
	const TSubclassOf<AGuideThread>& GuideClass
)
{
	PreviouGuideClass = nullptr;
	if (CurrentLineGuidePtr)
	{
		PreviouGuideClass = CurrentLineGuidePtr->GetClass();

		const auto CurrentGuideID = CurrentLineGuidePtr->GetGuideID();
		const auto CurrentTaskID = CurrentLineGuidePtr->GetCurrentTaskID();
		if (CurrentGuideID.IsValid() && CurrentTaskID.IsValid())
		{
			GuidePostionSet.Add(CurrentGuideID, CurrentTaskID);
		}
		
		OnStopGuide.Broadcast(CurrentLineGuidePtr);
		CurrentLineGuidePtr->Destroy();
	}

	FActorSpawnParameters SpawnParameters;

	auto GuideThreadPtr = GetWorld()->SpawnActor<AGuideThread>(
		GuideClass,
		SpawnParameters
	);

	CurrentLineGuidePtr = GuideThreadPtr;

	const auto NewGuideID = GuideClass.GetDefaultObject()->GetGuideID();
	if (!NewGuideID.IsValid())
	{
		checkNoEntry();
	}

	if (GuidePostionSet.Contains(NewGuideID))
	{
		//
		CurrentLineGuidePtr->SetPreviousTaskID(GuidePostionSet[NewGuideID]);
	}

	CurrentLineGuidePtr->GetGuideSystemStateTreeComponent()->StartLogic();

	OnStartGuide.Broadcast(CurrentLineGuidePtr);
}
