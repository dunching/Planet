#include "EndangeredStateLayout.h"

#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "GameplayCommand.h"
#include "GuideSubSystem.h"
#include "HumanCharacter_Player.h"
#include "GuideThreadChallenge.h"
#include "LayoutCommon.h"
#include "PlanetPlayerController.h"
#include "PlanetPlayerState.h"
#include "PlayerGameplayTasks.h"

struct FEndangeredStateLayout : public TStructVariable<FEndangeredStateLayout>
{
	FName RespawnBtn = TEXT("RespawnBtn");
};

void UEndangeredStateLayout::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIPtr = Cast<UButton>(GetWidgetFromName(FEndangeredStateLayout::Get().RespawnBtn));
	if (UIPtr)
	{
		UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
	}
}

void UEndangeredStateLayout::Enable()
{
}

void UEndangeredStateLayout::DisEnable()
{
}

ELayoutCommon UEndangeredStateLayout::GetLayoutType() const
{
	return ELayoutCommon::kEndangeredLayout;
}

void UEndangeredStateLayout::OnClicked()
{
	// 确认是否在挑战模式
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PCPtr)
	{
		if (PCPtr->GetPlayerState<APlanetPlayerState>()->GetIsInChallenge())
		{
			if (auto GuideThreadPtr = UGuideSubSystem::GetInstance()->IsActivedGuideThread(
				 AGuideThread_Challenge::StaticClass()
				))
			{
				auto GuideThread_ChallengePtr = Cast<AGuideThread_Challenge>(GuideThreadPtr);
				if (GuideThread_ChallengePtr)
				{
					PCPtr->GetGameplayTasksComponent()->EntryChallengeLevel(GuideThread_ChallengePtr->CurrentTeleport);
				}
			}
		}
		else
		{
			PCPtr->GetGameplayTasksComponent()->TeleportPlayerToNearest();
		}
	}
}
