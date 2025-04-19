#include "GameplayCommand.h"

#include "InputProcessorSubSystem.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "HumanCharacter_Player.h"
#include "HumanRegularProcessor.h"
#include "PlanetPlayerController.h"
#include "ChallengeSystem.h"
#include "GroupManagger.h"
#include "GuideSubSystem.h"
#include "HumanViewTalentAllocation.h"
#include "OpenWorldDataLayer.h"
#include "OpenWorldSystem.h"
#include "TeamMatesHelperComponent.h"

void GameplayCommand::ActiveGuideMainThread()
{
	UGuideSubSystem::GetInstance()->ActiveMainThread();
}

void GameplayCommand::ViewAllocationMenu()
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanViewAlloctionSkillsProcessor>();
}

void GameplayCommand::EntryActionProcessor()
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
}

void GameplayCommand::ViewGroupmateMenu()
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanViewTalentAllocation>();
}

void GameplayCommand::EntryChallengeLevel(const TArray<FString>& Args)
{
	if (Args.IsValidIndex(0))
	{
		auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
		if (PCPtr)
		{
			if (Args[0] == TEXT("ReturnOpenWorld"))
			{
				PCPtr->EntryChallengeLevel(ETeleport::kReturnOpenWorld);
			}
			else if (Args[0] == TEXT("Teleport"))
			{
				PCPtr->EntryChallengeLevel(ETeleport::kTeleport_1);
			}
			else if (Args[0] == TEXT("Test1"))
			{
				PCPtr->EntryChallengeLevel(ETeleport::kTest1);
			}
			else if (Args[0] == TEXT("Test2"))
			{
				PCPtr->EntryChallengeLevel(ETeleport::kTest2);
			}
		}
	}
}

void GameplayCommand::SwitchTeammateOptionToFollow()
{
	auto OnwerActorPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (OnwerActorPtr)
	{
		OnwerActorPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->SwitchTeammateOption(
			ETeammateOption::kFollow);
	}
}

void GameplayCommand::SwitchTeammateOptionTokAssistance()
{
	auto OnwerActorPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (OnwerActorPtr)
	{
		OnwerActorPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->SwitchTeammateOption(
			ETeammateOption::kAssistance);
	}
}

void GameplayCommand::RespawnPlayerCharacter()
{
	
}
