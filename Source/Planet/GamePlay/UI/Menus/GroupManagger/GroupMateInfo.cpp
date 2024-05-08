
#include "GroupMateInfo.h"

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

namespace GroupMateInfo
{
	const FName Texture = TEXT("Texture");

	const FName Text = TEXT("Text");

	const FName Content = TEXT("Content");

	const FName Default = TEXT("Default");
}

void UGroupMateInfo::InvokeReset(UUserWidget* BaseWidgetPtr)
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

void UGroupMateInfo::ResetToolUIByData(UBasicUnit* BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kGroupMate)
	{
		{
			auto BorderPtr = Cast<UBorder>(GetWidgetFromName(GroupMateInfo::Content));
			if (BorderPtr)
			{
				BorderPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
		{
			auto ImagePtr = Cast<UImage>(GetWidgetFromName(GroupMateInfo::Default));
			if (ImagePtr)
			{
				ImagePtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		GroupMateUnitPtr = Cast<UGourpMateUnit>(BasicUnitPtr);
		{
			auto UIPtr = Cast<UImage>(GetWidgetFromName(GroupMateInfo::Texture));
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
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(GroupMateInfo::Text));
			if (UIPtr)
			{
				UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%s(%d)"), *GroupMateUnitPtr->Name.ToString(), GroupMateUnitPtr->Level)));
			}
		}
	}
	else
	{
		{
			auto BorderPtr = Cast<UBorder>(GetWidgetFromName(GroupMateInfo::Content));
			if (BorderPtr)
			{
				BorderPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		{
			auto ImagePtr = Cast<UImage>(GetWidgetFromName(GroupMateInfo::Default));
			if (ImagePtr)
			{
				ImagePtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}
