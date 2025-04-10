#include "GuideSubSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Planet_Tools.h"
#include "PlanetWorldSettings.h"
#include "GuideActor.h"
#include "GuideSystemStateTreeComponent.h"
#include "GuideThread.h"

bool UGuideSubSystem::ShouldCreateSubsystem(UObject* Outer) const
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

	ActiveTargetGuideThread(WorldSetting->MainLineGuideClass, false);
}

void UGuideSubSystem::AddBranchLine(const TSubclassOf<AGuideBranchThread>& BranchLineGuideClass)
{
}

void UGuideSubSystem::ActiveTargetGuideThread(const TSubclassOf<AGuideThread>& GuideClass, bool bIsTransit)
{
	const auto NewGuideID = GuideClass.GetDefaultObject()->GetGuideID();
	if (!NewGuideID.IsValid())
	{
		checkNoEntry();
	}

	auto Lambda = [this, NewGuideID](auto Target)
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
			CurrentLineGuidePtr->Destroy();
		}

		CurrentLineGuidePtr = Target;

		if (GuidePostionSet.Contains(NewGuideID))
		{
			//
			CurrentLineGuidePtr->SetPreviousTaskID(GuidePostionSet[NewGuideID]);
		}
		
		CurrentLineGuidePtr->GetGuideSystemStateTreeComponent()->StartLogic();

		OnCurrentGuideChagned.Broadcast(CurrentLineGuidePtr);

		return true;
	};
	
	if (bIsTransit)
	{
		FActorSpawnParameters SpawnParameters;

		auto GuideThreadPtr = GetWorld()->SpawnActor<AGuideThread>(
			GuideClass, SpawnParameters
		);
		// BrandLineGuidePtr->GuideID = TargetID;
		
		Lambda(GuideThreadPtr);
		return;
	}
	
	FActorSpawnParameters SpawnParameters;

	auto GuideThreadPtr = GetWorld()->SpawnActor<AGuideThread>(
		GuideClass, SpawnParameters
	);
		
	Lambda(GuideThreadPtr);
}

void UGuideSubSystem::StopActiveTargetGuideThread(const TSubclassOf<AGuideThread>& GuideClass)
{
	if (CurrentLineGuidePtr)
	{
		if (CurrentLineGuidePtr->IsA(GuideClass))
		{
			GuidePostionSet.Add(CurrentLineGuidePtr->GetPreviousTaskID());
			CurrentLineGuidePtr->Destroy();
			
			CurrentLineGuidePtr = nullptr;
			PreviouGuideClass = nullptr;

			OnCurrentGuideChagned.Broadcast(CurrentLineGuidePtr);
		}
	}
}

void UGuideSubSystem::GuideThreadEnded(AGuideThread* GuideThreadPtr)
{
	OnGuideEnd.Broadcast(GuideThreadPtr);

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

FOnGuideEnd& UGuideSubSystem::GetOnGuideEnd() 
{
	return OnGuideEnd;
}
