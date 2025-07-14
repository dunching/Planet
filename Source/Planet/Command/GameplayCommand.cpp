#include "GameplayCommand.h"

#include "Kismet/KismetMathLibrary.h"

#include "InputProcessorSubSystemBase.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "HumanCharacter_Player.h"
#include "HumanRegularProcessor.h"
#include "PlanetPlayerController.h"
#include "ChallengeSystem.h"
#include "GroupManagger.h"
#include "QuestSubSystem.h"
#include "HumanViewSetting.h"
#include "HumanViewTalentAllocation.h"
#include "InputProcessorSubSystem_Imp.h"
#include "OpenWorldDataLayer.h"
#include "OpenWorldSystem.h"
#include "TeamMatesHelperComponent.h"
#include "TeamMatesHelperComponentBase.h"
#include "PlanetLocalPlayer.h"

void GameplayCommand::CloneCharacter()
{
	auto PlanetLocalPlayerPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));

	if (PlanetLocalPlayerPtr)
	{
		auto PCPtr = PlanetLocalPlayerPtr;
		auto Rot = PCPtr->GetControlRotation().Quaternion();
		auto Transform = PCPtr->GetPawn()->GetTransform();
		Transform.AddToTranslation( Rot.GetAxisY() * 100);
		PlanetLocalPlayerPtr->CloneCharacter_Server(FGuid::NewGuid(), Transform,ETeammateOption::kAssistance);
	}
}

void GameplayCommand::ActiveGuideMainThread()
{
	UQuestSubSystem::GetInstance()->ActiveMainThread();
}

void GameplayCommand::ViewAllocationMenu()
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanViewAlloctionSkillsProcessor>();
}

void GameplayCommand::EntryActionProcessor()
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
}

void GameplayCommand::ViewGroupmateMenu()
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanViewTalentAllocation>();
}

void GameplayCommand::ViewSetting()
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanViewSetting>();
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
				PCPtr->EntryChallengeLevel(ETeleport::kTeleport_NoviceVillage);
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
		OnwerActorPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->SwitchTeammateOption_Server(
			 ETeammateOption::kFollow
			);
	}
}

void GameplayCommand::SwitchTeammateOptionToAssistance()
{
	auto OnwerActorPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (OnwerActorPtr)
	{
		OnwerActorPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->SwitchTeammateOption_Server(
			 ETeammateOption::kAssistance
			);
	}
}

void GameplayCommand::SwitchTeammateOptionToFireTarget()
{
	auto OnwerActorPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (OnwerActorPtr)
	{
		OnwerActorPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->SwitchTeammateOption_Server(
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

			PCPtr->SpawnCharacterByProxyType_Server(FGameplayTag::RequestGameplayTag(*Args[0]), Transform);
		}
	}
}
