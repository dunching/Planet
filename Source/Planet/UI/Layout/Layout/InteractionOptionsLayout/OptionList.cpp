#include "OptionList.h"

#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBox.h"

#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "OptionItem.h"

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

void UOptionList::Enable()
{
	ILayoutItemInterfacetion::Enable();
}

void UOptionList::DisEnable()
{
	ILayoutItemInterfacetion::DisEnable();
}

void UOptionList::UpdateDisplay(
	AHumanCharacter_AI* InTargetCharacterPtr,
	const std::function<void(const TSubclassOf<AQuestInteractionBase>&)>& InCallback
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

	const auto TaskNodes = TargetCharacterPtr->GetSceneActorInteractionComponent()->GetInteractionLists();
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
	ISceneActorInteractionInterface* InSceneActorInteractionInterfacePtr,
	const std::function<void(const TSubclassOf<AQuestInteractionBase>&)>& InCallback
)
{
	SceneActorInteractionInterfacePtr = InSceneActorInteractionInterfacePtr;
	if (!SceneActorInteractionInterfacePtr)
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

	const auto TaskNodes = SceneActorInteractionInterfacePtr->GetSceneActorInteractionComponent()->GetInteractionLists();
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

void UOptionList::UpdateDisplay(const TArray<FString>& OptionAry, const std::function<void(int32)>& InCallback)
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
	for (const auto& Iter : OptionAry)
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
