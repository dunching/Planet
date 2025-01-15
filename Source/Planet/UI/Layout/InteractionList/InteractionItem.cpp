#include "InteractionItem.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "TaskNode_SceneActor.h"
#include "GuideActor.h"
#include "InputProcessorSubSystem.h"
#include "InteractionList.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "PlanetPlayerController.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter_Player.h"

namespace HumanProcessor
{
	class FHumanRegularProcessor;
}

class AMainHUD;

struct FInteractionItem : public TStructVariable<FInteractionItem>
{
	const FName Text = TEXT("Text");

	const FName Btn = TEXT("Btn");
};

void UInteractionItem::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIPtr = Cast<UButton>(GetWidgetFromName(FInteractionItem::Get().Btn));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
}

void UInteractionItem::ResetUIByData()
{
}

void UInteractionItem::SetData(const TSubclassOf<AGuideInteractionActor>& InTaskNode)
{
	if (!InTaskNode.Get())
	{
		return;
	}

	TaskNode = InTaskNode;

	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FInteractionItem::Get().Text));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->SetText(FText::FromString(TaskNode.GetDefaultObject()->TaskName));
}

void UInteractionItem::OnClicked()
{
	FActorSpawnParameters SpawnParameters;

	auto GuideInteractionActorPtr = GetWorld()->SpawnActor<AGuideInteractionActor>(
		TaskNode, SpawnParameters
	);

	if (GuideInteractionActorPtr)
	{
		GuideInteractionActorPtr->OnGuideInteractionEnd.AddLambda([]
		{
			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
		});
		
		Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))
			->GetHUD<AMainHUD>()
			->GetMainHUDLayout()
			->GetInteractionList()
			->CloseUI();
	}
}
