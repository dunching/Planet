#include "InteractionList.h"

#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBox.h"

#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "InteractionItem.h"

struct FInteractionList : public TStructVariable<FInteractionList>
{
	const FName VerticalBox = TEXT("VerticalBox");
};

void UInteractionList::NativeConstruct()
{
	Super::NativeConstruct();

	// SetAnchorsInViewport(FAnchors(.5f));
	// SetAlignmentInViewport(FVector2D(.5f, .5f));

	CloseUI();

	// TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition));
	// ResetPosition(0.f);
}

void UInteractionList::NativeDestruct()
{
	Super::NativeDestruct();
}

void UInteractionList::ResetUIByData()
{
}

void UInteractionList::UpdateDisplay(AHumanCharacter_AI* InTargetCharacterPtr)
{
	TargetCharacterPtr = InTargetCharacterPtr;
	if (!TargetCharacterPtr)
	{
		return;
	}

	SetVisibility(ESlateVisibility::Visible);

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FInteractionList::Get().VerticalBox));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->ClearChildren();

	const auto TaskNodes = TargetCharacterPtr->GetSceneActorInteractionComponent()->GetTaskNodes();
	for (const auto& Iter : TaskNodes)
	{
	}

	// 按F互动的条目
	{
		auto ItemUIPtr = CreateWidget<UInteractionItem>(GetWorld(), InteractionItemClass);
		if (ItemUIPtr)
		{
			// ItemUIPtr->SetData(Iter);
			UIPtr->AddChild(ItemUIPtr);
		}
	}
}

void UInteractionList::CloseUI()
{
	SetVisibility(ESlateVisibility::Hidden);
}

bool UInteractionList::ResetPosition(float InDeltaTime)
{
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	// UGameplayStatics::ProjectWorldToScreen(
	// 	UGameplayStatics::GetPlayerController(this, 0),
	// 	TargetCharacterPtr->GetActorLocation(),
	// 	ScreenPosition
	// );

	SetPositionInViewport(ScreenPosition);

	return true;
}
