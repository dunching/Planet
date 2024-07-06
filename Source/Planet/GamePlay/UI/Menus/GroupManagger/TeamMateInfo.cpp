
#include "TeamMateInfo.h"

#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Border.h"

#include <CacheAssetManager.h>
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "SceneElement.h"
#include "CharacterBase.h"
#include "HumanControllerInterface.h"
#include "GroupMnaggerComponent.h"

namespace TeamMateInfo
{
	const FName Texture = TEXT("Texture");

	const FName Text = TEXT("Text");

	const FName Content = TEXT("Content");

	const FName Default = TEXT("Default");
}

FReply UTeamMateInfo::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		ResetToolUIByData(nullptr);
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool UTeamMateInfo::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (InOperation->IsA(UItemsDragDropOperation::StaticClass()))
	{
		auto WidgetDragPtr = Cast<UItemsDragDropOperation>(InOperation);
		if (WidgetDragPtr)
		{
			ResetToolUIByData(WidgetDragPtr->SceneToolSPtr);
			AddMember();
			if (GroupMateUnitPtr)
			{
				OnDroped.ExcuteCallback(this);
			}
		}
	}

	return true;
}

void UTeamMateInfo::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->GroupMateUnitPtr);
		}
	}
}

void UTeamMateInfo::ResetToolUIByData(UBasicUnit* BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kGroupMate)
	{
		{
			auto BorderPtr = Cast<UBorder>(GetWidgetFromName(TeamMateInfo::Content));
			if (BorderPtr)
			{
				BorderPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
		{
			auto ImagePtr = Cast<UImage>(GetWidgetFromName(TeamMateInfo::Default));
			if (ImagePtr)
			{
				ImagePtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		
		GroupMateUnitPtr = Cast<UGourpmateUnit>(BasicUnitPtr);
		{
			auto UIPtr = Cast<UImage>(GetWidgetFromName(TeamMateInfo::Texture));
			if (UIPtr)
			{
				FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
				AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(GroupMateUnitPtr->GetIcon().ToSoftObjectPath(), [this, UIPtr]()
					{
						UIPtr->SetBrushFromTexture(GroupMateUnitPtr->GetIcon().Get());
					});
			}
		}
		{
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(TeamMateInfo::Text));
			if (UIPtr)
			{
				UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%s(%d)"), *GroupMateUnitPtr->Name.ToString(), GroupMateUnitPtr->Level)));
			}
		}
	}
	else
	{
		{
			auto BorderPtr = Cast<UBorder>(GetWidgetFromName(TeamMateInfo::Content));
			if (BorderPtr)
			{
				BorderPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		{
			auto ImagePtr = Cast<UImage>(GetWidgetFromName(TeamMateInfo::Default));
			if (ImagePtr)
			{
				ImagePtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UTeamMateInfo::EnableIcon(bool bIsEnable)
{

}

void UTeamMateInfo::AddMember()
{
	auto PCPtr = Cast<IPlanetControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}
	auto GMCPtr = PCPtr->GetGroupMnaggerComponent();

	auto TeamsHelperSPtr = GMCPtr->GetTeamHelper();
	if (TeamsHelperSPtr)
	{
		TeamsHelperSPtr->AddCharacter(GroupMateUnitPtr, GroupMateUnitPtr->PCPtr);
	}
}
