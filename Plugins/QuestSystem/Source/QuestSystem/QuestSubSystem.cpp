#include "QuestSubSystem.h"

#include "QuestChainBase.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Tools.h"

bool UQuestSubSystem::ShouldCreateSubsystem(
	UObject* Outer
	) const
{
	if (IsRunningDedicatedServer())
	{
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}

UQuestSubSystem* UQuestSubSystem::GetInstance()
{
	auto WorldSetting = Cast<IGetQuestSubSystemInterface>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetGuideSubSystem();
}

void UQuestSubSystem::InitializeMainThread()
{
}

void UQuestSubSystem::ActiveMainThread()
{
	ActiveTargetGuideThread(GetCurrentMainGuideThread());
}

void UQuestSubSystem::AddGuideThread_Brand(
	const TSubclassOf<AQuestChain_BranchBase>& BranchLineGuideClass
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

void UQuestSubSystem::ActiveBrandGuideThread(
	const TSubclassOf<AQuestChain_BranchBase>& GuideClass
	)
{
	ActiveTargetGuideThread(GuideClass);
}

void UQuestSubSystem::StopActiveBrandGuideThread(
	const TSubclassOf<AQuestChain_BranchBase>& GuideClass
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

void UQuestSubSystem::StartParallelGuideThread(
	const TSubclassOf<AQuestChain_ImmediateBase>& GuideClass
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

	auto GuideThreadPtr = GetWorld()->SpawnActor<AQuestChainBase>(
	                                                              GuideClass,
	                                                              SpawnParameters
	                                                             );

	GuideThreadPtr->ActiveGuide();

	OnStartGuide.Broadcast(GuideThreadPtr);
	ActivedGuideThreadsAry.Add(GuideThreadPtr);
}

void UQuestSubSystem::StopParallelGuideThread(
	const TSubclassOf<AQuestChain_ImmediateBase>& GuideClass
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

void UQuestSubSystem::OnGuideThreadStoped(
	AQuestChainBase* GuideThreadPtr
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

			if (GuideThreadPtr->IsA(AQuestChain_MainBase::StaticClass()))
			{
				CurrentMainGuideThreadIndex++;
				ActiveMainThread();
			}
		}

		GuideThreadPtr->Destroy();
		GuideThreadPtr = nullptr;
	}
}

TObjectPtr<AQuestChainBase> UQuestSubSystem::GetCurrentGuideThread() const
{
	// 优先返回临时的任务
	for (auto Iter : ActivedGuideThreadsAry)
	{
		if (Iter == CurrentLineGuidePtr)
		{
			continue;
		}
		else
		{
			return Iter;
		}
	}

	// 若没有临时的任务，则返回主线或者支线
	return CurrentLineGuidePtr;
}

TSubclassOf<AQuestChain_MainBase> UQuestSubSystem::GetCurrentMainGuideThread() const
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

TArray<TSubclassOf<AQuestChain_BranchBase>> UQuestSubSystem::GetGuideThreadAry() const
{
	return GuideThreadBranchAry;
}

TObjectPtr<AQuestChainBase> UQuestSubSystem::IsActivedGuideThread(
	const TSubclassOf<AQuestChain_ImmediateBase>& GuideClass
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

const TArray<TObjectPtr<AQuestChainBase>>& UQuestSubSystem::GetActivedGuideThreadsAry() const
{
	return ActivedGuideThreadsAry;
}

FOnStartGuide& UQuestSubSystem::GetOnStartGuide()
{
	return OnStartGuide;
}

FOnStopGuide& UQuestSubSystem::GetOnStopGuide()
{
	return OnStopGuide;
}

void UQuestSubSystem::AddGuidePostion(
	const FGuid& CurrentGuideID,
	const FGuid& CurrentTaskID
	)
{
	if (CurrentGuideID.IsValid() && CurrentTaskID.IsValid())
	{
		GuidePostionSet.Add(CurrentGuideID, CurrentTaskID);
	}
}

bool UQuestSubSystem::ConsumeGuidePostion(
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

void UQuestSubSystem::RemoveGuidePostion(
	const FGuid& CurrentGuideID
	)
{
	if (GuidePostionSet.Contains(CurrentGuideID))
	{
		//
		GuidePostionSet.Remove(CurrentGuideID);
	}
}

TArray<TSubclassOf<AQuestChain_MainBase>> UQuestSubSystem::GetMainGuideThreadChaptersAry() const
{
	return {};
}

TSubclassOf<AQuestChain_MainBase> UQuestSubSystem::GetToBeContinueGuideThread() const
{
	return nullptr;
}

void UQuestSubSystem::ActiveTargetGuideThread(
	const TSubclassOf<AQuestChainBase>& GuideClass
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

	auto GuideThreadPtr = GetWorld()->SpawnActor<AQuestChainBase>(
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
