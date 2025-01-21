#include "OptionList.h"

#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBox.h"

#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "OptionItem.h"
#include "TaskNode_Interaction.h"
#include "TextCollect.h"
#include "TextSubSystem.h"

struct FOptionList : public TStructVariable<FOptionList>
{
	const FName VerticalBox = TEXT("VerticalBox");
};

void UOptionList::NativeConstruct()
{
	Super::NativeConstruct();

	// SetAnchorsInViewport(FAnchors(.5f));
	// SetAlignmentInViewport(FVector2D(.5f, .5f));

	CloseUI();

	// TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition));
	// ResetPosition(0.f);
}

void UOptionList::NativeDestruct()
{
	Super::NativeDestruct();
}

void UOptionList::ResetUIByData()
{
}

void UOptionList::UpdateDisplay(
	AHumanCharacter_AI* InTargetCharacterPtr,
	const std::function<void(const TSubclassOf<AGuideInteractionActor>&)>& InCallback
)
{
	TargetCharacterPtr = InTargetCharacterPtr;
	if (!TargetCharacterPtr)
	{
		return;
	}

	SetVisibility(ESlateVisibility::Visible);

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FOptionList::Get().VerticalBox));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->ClearChildren();

	const auto TaskNodes = TargetCharacterPtr->GetSceneActorInteractionComponent()->GetTaskNodes();
	for (const auto& Iter : TaskNodes)
	{
		auto ItemUIPtr = CreateWidget<UOptionItem>(GetWorld(), InteractionItemClass);
		if (ItemUIPtr)
		{
			ItemUIPtr->SetData(Iter, InCallback);
			UIPtr->AddChild(ItemUIPtr);
		}
	}
}

void UOptionList::UpdateDisplay(
	const TSoftObjectPtr<UPAD_TaskNode_Interaction_Option>& InTaskNodeRef,
	const std::function<void(int32)>& InCallback
)
{
	TaskNodeRef = InTaskNodeRef;
	if (TaskNodeRef.IsValid())
	{
		SetVisibility(ESlateVisibility::Visible);

		auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FOptionList::Get().VerticalBox));
		if (!UIPtr)
		{
			return;
		}

		UIPtr->ClearChildren();

		// 选项
		int32 Index = 1;
		for (const auto& Iter : TaskNodeRef.LoadSynchronous()->OptionAry)
		{
			auto ItemUIPtr = CreateWidget<UOptionItem>(GetWorld(), InteractionItemClass);
			if (ItemUIPtr)
			{
				ItemUIPtr->SetData(Iter, Index, InCallback);
				UIPtr->AddChild(ItemUIPtr);
			}
			Index++;
		}

		// “取消”选项
		Index = 0;
		auto ItemUIPtr = CreateWidget<UOptionItem>(GetWorld(), InteractionItemClass);
		if (ItemUIPtr)
		{
			ItemUIPtr->SetData(UTextSubSystem::GetInstance()->GetText(TextCollect::Return), Index, InCallback);
			UIPtr->AddChild(ItemUIPtr);
		}
	}
}

void UOptionList::CloseUI()
{
	SetVisibility(ESlateVisibility::Hidden);

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FOptionList::Get().VerticalBox));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->ClearChildren();
}

bool UOptionList::ResetPosition(float InDeltaTime)
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
