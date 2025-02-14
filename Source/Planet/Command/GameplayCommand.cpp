
#include "GameplayCommand.h"

#include "InputProcessorSubSystem.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "HumanCharacter_Player.h"
#include "HumanRegularProcessor.h"
#include "PlanetPlayerController.h"
#include "ChallengeSystem.h"

void GameplayCommand::ViewAllocationMenu()
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanViewAlloctionSkillsProcessor>();
}

void GameplayCommand::EntryActionProcessor()
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
}

void GameplayCommand::EntryChallengeLevel(const TArray<FString>& Args)
{
	if (Args.IsValidIndex(0))
	{
		auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
		if (PCPtr->HasAuthority())
		{
		}
		PCPtr->EntryChallengeLevel(Args);
	}
}
