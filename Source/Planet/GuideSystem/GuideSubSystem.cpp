#include "GuideSubSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Planet_Tools.h"
#include "PlanetWorldSettings.h"
#include "GuideActor.h"
#include "GuideSystemStateTreeComponent.h"
#include "GuideThread.h"
#include "PlanetPlayerController.h"
#include "Kismet/GameplayStatics.h"

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
	                             USubsystemBlueprintLibrary::GetWorldSubsystem(
	                                                                           GetWorldImp(),
	                                                                           UGuideSubSystem::StaticClass()
	                                                                          )
	                            );
}

void UGuideSubSystem::InitializeMainThread()
{
}

void UGuideSubSystem::ActiveMainThread()
{
	ActiveTargetGuideThread(GetCurrentMainGuideThread());
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
			AddGuidePostion(CurrentLineGuidePtr->GetGuideID(), CurrentLineGuidePtr->GetPreviousTaskID());
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

	GuideThreadPtr->ActiveGuide();

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

void UGuideSubSystem::OnGuideThreadStoped(
	AGuideThread* GuideThreadPtr
	)
{
	OnStopGuide.Broadcast(GuideThreadPtr);

	if (GuideThreadPtr)
	{
		// 任务已完成
		if (GuideThreadPtr->bIsComleted)
		{
			auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
			if (PCPtr)
			{
				for (const auto& Iter : GuideThreadPtr->RewardProxysMap)
				{
					PCPtr->AddProxy(Iter.Key, Iter.Value);
				}
			}

			const auto CurrentGuideID = GuideThreadPtr->GetGuideID();
			const auto CurrentTaskID = GuideThreadPtr->GetCurrentTaskID();

			RemoveGuidePostion(CurrentGuideID);

			if (GuideThreadPtr->IsA(AGuideThread_Main::StaticClass()))
			{
				CurrentMainGuideThreadIndex++;
				ActiveMainThread();
			}
		}

		GuideThreadPtr->Destroy();
		GuideThreadPtr = nullptr;
	}
}

TObjectPtr<AGuideThread> UGuideSubSystem::GetCurrentGuideThread() const
{
	return CurrentLineGuidePtr;
}

TSubclassOf<AGuideThread_Main> UGuideSubSystem::GetCurrentMainGuideThread() const
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());

	if (WorldSetting->MainGuideThreadChaptersAry.IsValidIndex(CurrentMainGuideThreadIndex))
	{
		return WorldSetting->MainGuideThreadChaptersAry[CurrentMainGuideThreadIndex];
	}
	else
	{
		return WorldSetting->ToBeContinueGuideThread;
	}
}

TArray<TSubclassOf<AGuideThread_Branch>> UGuideSubSystem::GetGuideThreadAry() const
{
	return GuideThreadBranchAry;
}

TObjectPtr<AGuideThread> UGuideSubSystem::IsActivedGuideThread(
	const TSubclassOf<AGuideThread_Immediate>& GuideClass
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
		AddGuidePostion(CurrentGuideID, CurrentTaskID);

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

	FGuid CurrentTaskID;
	if (ConsumeGuidePostion(NewGuideID, CurrentTaskID))
	{
		//
		CurrentLineGuidePtr->SetPreviousTaskID(CurrentTaskID);
	}

	CurrentLineGuidePtr->ActiveGuide();

	OnStartGuide.Broadcast(CurrentLineGuidePtr);
}
