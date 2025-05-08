#include "GameplayCommand.h"

#include "Kismet/KismetMathLibrary.h"

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

void GameplayCommand::EntryChallengeLevel(
	const TArray<FString>& Args
	)
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
		OnwerActorPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->SwitchTeammateOption(
			 ETeammateOption::kFollow
			);
	}
}

void GameplayCommand::SwitchTeammateOptionToAssistance()
{
	auto OnwerActorPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (OnwerActorPtr)
	{
		OnwerActorPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->SwitchTeammateOption(
			 ETeammateOption::kAssistance
			);
	}
}

void GameplayCommand::SwitchTeammateOptionToFireTarget()
{
	auto OnwerActorPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (OnwerActorPtr)
	{
		OnwerActorPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->SwitchTeammateOption(
			 ETeammateOption::kFireTarget
			);
	}
}

void GameplayCommand::RespawnPlayerCharacter()
{
}

void GameplayCommand::ServerSpawnCharacterByProxyType(
	const TArray<FString>& Args
	)
{
	if (Args.IsValidIndex(0))
	{
		auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
		if (PCPtr)
		{
			FTransform Transform = FTransform::Identity;
			Transform.SetLocation(
			                      PCPtr->GetPawn()->GetActorLocation() + (PCPtr->GetPawn()->
			                                                              GetActorForwardVector() * 100)
			                     );
			Transform.SetRotation(
			UKismetMathLibrary::MakeRotFromZX(FVector::UpVector, -PCPtr->GetPawn()->GetActorForwardVector()).Quaternion()
			                     );

			PCPtr->ServerSpawnCharacterByProxyType(FGameplayTag::RequestGameplayTag(*Args[0]), Transform);
		}
	}
}
