
#include "HUD_TeamMateInfo.h"

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
#include "PlanetControllerInterface.h"
#include "HUD_TeamMateInfo.h"

namespace HUD_TeamMateInfo
{
	const FName Texture = TEXT("Texture");

	const FName Text = TEXT("Text");

	const FName Content = TEXT("Content");

	const FName Default = TEXT("Default");

	const FName TalentStateSocket = TEXT("TalentStateSocket");
}

void UHUD_TeamMateInfo::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

void UHUD_TeamMateInfo::InvokeReset(UUserWidget* BaseWidgetPtr)
{

}

void UHUD_TeamMateInfo::ResetToolUIByData(UBasicUnit* BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kGroupMate)
	{
		{
			auto BorderPtr = Cast<UBorder>(GetWidgetFromName(HUD_TeamMateInfo::Content));
			if (BorderPtr)
			{
				BorderPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
		{
			auto ImagePtr = Cast<UImage>(GetWidgetFromName(HUD_TeamMateInfo::Default));
			if (ImagePtr)
			{
				ImagePtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		GroupMateUnitPtr = Cast<UGourpmateUnit>(BasicUnitPtr);
		{
			auto UIPtr = Cast<UImage>(GetWidgetFromName(HUD_TeamMateInfo::Texture));
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
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(HUD_TeamMateInfo::Text));
			if (UIPtr)
			{
				UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%s(%d)"), *GroupMateUnitPtr->Name.ToString(), GroupMateUnitPtr->Level)));
			}
		}

		auto PCPtr = Cast<IPlanetControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
		if (!PCPtr)
		{
			return;
		}
	}
	else
	{
		{
			auto BorderPtr = Cast<UBorder>(GetWidgetFromName(HUD_TeamMateInfo::Content));
			if (BorderPtr)
			{
				BorderPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		{
			auto ImagePtr = Cast<UImage>(GetWidgetFromName(HUD_TeamMateInfo::Default));
			if (ImagePtr)
			{
				ImagePtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

