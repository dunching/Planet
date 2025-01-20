#include "OptionItem.h"

#include "ConversationLayout.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "TaskNode_Interaction.h"
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

void UOptionItem::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIPtr = Cast<UButton>(GetWidgetFromName(FInteractionItem::Get().Btn));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
}

void UOptionItem::NativeDestruct()
{
	Super::NativeDestruct();
}

void UOptionItem::ResetUIByData()
{
}

void UOptionItem::SetData(
	const TSubclassOf<AGuideInteractionActor>& InTaskNode,
	const std::function<void(const TSubclassOf<AGuideInteractionActor>&)>& InCallback
)
{
	if (!InTaskNode.Get())
	{
		return;
	}

	OnClickedInteractionItem.AddLambda(InCallback);
	TaskNode = InTaskNode;

	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FInteractionItem::Get().Text));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->SetText(FText::FromString(TaskNode.GetDefaultObject()->TaskName));
}

void UOptionItem::SetData(const FString& InOption, int32 InIndex, const std::function<void(int32)>& InOnClickedIndex)
{
	OnClickedIndex.AddLambda(InOnClickedIndex);
	Option = InOption;
	Index = InIndex;

	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FInteractionItem::Get().Text));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->SetText(FText::FromString(Option));
}

void UOptionItem::OnClicked()
{
	if (TaskNode)
	{
		OnClickedInteractionItem.Broadcast(TaskNode);
	}
	else
	{
		OnClickedIndex.Broadcast(Index);
	}
}
