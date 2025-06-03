#include "GuideSubSystem.h"

#include "GuideThreadBase.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Tools.h"

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
	auto WorldSetting = Cast<IGetGuideSubSystemInterface>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetGuideSubSystem();
}

void UGuideSubSystem::InitializeMainThread()
{
}

void UGuideSubSystem::ActiveMainThread()
{
	ActiveTargetGuideThread(GetCurrentMainGuideThread());
}

void UGuideSubSystem::AddGuideThread_Brand(
	const TSubclassOf<AGuideThread_BranchBase>& BranchLineGuideClass
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
	const TSubclassOf<AGuideThread_BranchBase>& GuideClass
	)
{
	ActiveTargetGuideThread(GuideClass);
}

void UGuideSubSystem::StopActiveBrandGuideThread(
	const TSubclassOf<AGuideThread_BranchBase>& GuideClass
	)
{
	if (CurrentLineGuidePtr)
	{
		if (CurrentLineGuidePtr->IsA(GuideClass))
		{
			AddGuidePostion(CurrentLineGuidePtr->GetGuideID(), CurrentLineGuidePtr->GetPreviousTaskID());
			CurrentLineGuidePtr->Destroy();

			CurrentLineGuidePtr = nullptr;
			PreviouGuideClass = nullptr;
		}
	}
}

void UGuideSubSystem::StartParallelGuideThread(
	const TSubclassOf<AGuideThread_ImmediateBase>& GuideClass
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

	auto GuideThreadPtr = GetWorld()->SpawnActor<AGuideThreadBase>(
	                                                           GuideClass,
	                                                           SpawnParameters
	                                                          );

	GuideThreadPtr->ActiveGuide();

	OnStartGuide.Broadcast(GuideThreadPtr);
	ActivedGuideThreadsAry.Add(GuideThreadPtr);
}

void UGuideSubSystem::StopParallelGuideThread(
	const TSubclassOf<AGuideThread_ImmediateBase>& GuideClass
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

void UGuideSubSystem::OnGuideThreadStoped(
	AGuideThreadBase* GuideThreadPtr
	)
{
	OnStopGuide.Broadcast(GuideThreadPtr);

	if (GuideThreadPtr)
	{
		// 任务已完成
		if (GuideThreadPtr->bIsComleted)
		{
			// TODO

			const auto CurrentGuideID = GuideThreadPtr->GetGuideID();
			const auto CurrentTaskID = GuideThreadPtr->GetCurrentTaskID();

			RemoveGuidePostion(CurrentGuideID);

			if (GuideThreadPtr->IsA(AGuideThread_MainBase::StaticClass()))
			{
				CurrentMainGuideThreadIndex++;
				ActiveMainThread();
			}
		}

		GuideThreadPtr->Destroy();
		GuideThreadPtr = nullptr;
	}
}

TObjectPtr<AGuideThreadBase> UGuideSubSystem::GetCurrentGuideThread() const
{
	return CurrentLineGuidePtr;
}

TSubclassOf<AGuideThread_MainBase> UGuideSubSystem::GetCurrentMainGuideThread() const
{
	auto MainGuideThreadChaptersAry = GetMainGuideThreadChaptersAry();

	if (MainGuideThreadChaptersAry.IsValidIndex(CurrentMainGuideThreadIndex))
	{
		return MainGuideThreadChaptersAry[CurrentMainGuideThreadIndex];
	}
	else
	{
		return GetToBeContinueGuideThread();
	}
}

TArray<TSubclassOf<AGuideThread_BranchBase>> UGuideSubSystem::GetGuideThreadAry() const
{
	return GuideThreadBranchAry;
}

TObjectPtr<AGuideThreadBase> UGuideSubSystem::IsActivedGuideThread(
	const TSubclassOf<AGuideThread_ImmediateBase>& GuideClass
	) const
{
	for (auto Iter : ActivedGuideThreadsAry)
	{
		if (Iter->IsA(GuideClass))
		{
			return Iter;
		}
	}

	return nullptr;
}

FOnStartGuide& UGuideSubSystem::GetOnStartGuide()
{
	return OnStartGuide;
}

FOnStopGuide& UGuideSubSystem::GetOnStopGuide()
{
	return OnStopGuide;
}

void UGuideSubSystem::AddGuidePostion(
	const FGuid& CurrentGuideID,
	const FGuid& CurrentTaskID
	)
{
	if (CurrentGuideID.IsValid() && CurrentTaskID.IsValid())
	{
		GuidePostionSet.Add(CurrentGuideID, CurrentTaskID);
	}
}

bool UGuideSubSystem::ConsumeGuidePostion(
	const FGuid& CurrentGuideID,
	FGuid& CurrentTaskID
	)
{
	if (GuidePostionSet.Contains(CurrentGuideID))
	{
		//
		CurrentTaskID = GuidePostionSet[CurrentGuideID];
		GuidePostionSet.Remove(CurrentGuideID);
		return true;
	}

	return false;
}

void UGuideSubSystem::RemoveGuidePostion(
	const FGuid& CurrentGuideID
	)
{
	if (GuidePostionSet.Contains(CurrentGuideID))
	{
		//
		GuidePostionSet.Remove(CurrentGuideID);
	}
}

TArray<TSubclassOf<AGuideThread_MainBase>> UGuideSubSystem::GetMainGuideThreadChaptersAry() const
{
	return {};
}

TSubclassOf<AGuideThread_MainBase> UGuideSubSystem::GetToBeContinueGuideThread() const
{
	return nullptr;
}

void UGuideSubSystem::ActiveTargetGuideThread(
	const TSubclassOf<AGuideThreadBase>& GuideClass
	)
{
	PreviouGuideClass = nullptr;
	if (CurrentLineGuidePtr)
	{
		PreviouGuideClass = CurrentLineGuidePtr->GetClass();

		const auto CurrentGuideID = CurrentLineGuidePtr->GetGuideID();
		const auto CurrentTaskID = CurrentLineGuidePtr->GetCurrentTaskID();
		AddGuidePostion(CurrentGuideID, CurrentTaskID);

		OnStopGuide.Broadcast(CurrentLineGuidePtr);
		CurrentLineGuidePtr->Destroy();
	}

	FActorSpawnParameters SpawnParameters;

	auto GuideThreadPtr = GetWorld()->SpawnActor<AGuideThreadBase>(
	                                                           GuideClass,
	                                                           SpawnParameters
	                                                          );

	CurrentLineGuidePtr = GuideThreadPtr;

	const auto NewGuideID = GuideClass.GetDefaultObject()->GetGuideID();
	if (!NewGuideID.IsValid())
	{
		checkNoEntry();
	}

	FGuid CurrentTaskID;
	if (ConsumeGuidePostion(NewGuideID, CurrentTaskID))
	{
		//
		CurrentLineGuidePtr->SetPreviousTaskID(CurrentTaskID);
	}

	CurrentLineGuidePtr->ActiveGuide();

	OnStartGuide.Broadcast(CurrentLineGuidePtr);
}
