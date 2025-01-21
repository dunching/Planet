#include "GuideSubSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Planet_Tools.h"
#include "PlanetWorldSettings.h"
#include "GuideActor.h"
#include "GuideSystemStateTreeComponent.h"
#include "GuideThreadActor.h"

UGuideSubSystem* UGuideSubSystem::GetInstance()
{
	return Cast<UGuideSubSystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(GetWorldImp(), UGuideSubSystem::StaticClass())
	);
}

void UGuideSubSystem::InitializeMainLine()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());

	FActorSpawnParameters SpawnParameters;

	MainLineGuidePtr = GetWorld()->SpawnActor<AGuideMainThread>(
		WorldSetting->MainLineGuideClass, SpawnParameters
	);
}

void UGuideSubSystem::ActiveMainLine()
{
	if (CurrentLineGuidePtr)
	{
		CurrentLineGuidePtr->GetGuideSystemStateTreeComponent()->StopLogic(TEXT(""));
	}

	CurrentLineGuidePtr = MainLineGuidePtr;
	CurrentLineGuidePtr->GetGuideSystemStateTreeComponent()->StartLogic();

	OnCurrentGuideChagned.Broadcast(CurrentLineGuidePtr);
}

void UGuideSubSystem::AddBranchLine(const TSubclassOf<AGuideBranchThread>& BranchLineGuideClass)
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());

	FActorSpawnParameters SpawnParameters;

	BranchLineAry.Add(GetWorld()->SpawnActor<AGuideBranchThread>(
		WorldSetting->MainLineGuideClass, SpawnParameters
	));
}

TObjectPtr<AGuideThread> UGuideSubSystem::GetCurrentGuideThread() const
{
	return CurrentLineGuidePtr;
}
