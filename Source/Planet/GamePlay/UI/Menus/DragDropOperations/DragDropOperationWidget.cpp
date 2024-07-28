
#include "DragDropOperationWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "BackpackIcon.h"

#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "SceneElement.h"

UDragDropOperationWidget::UDragDropOperationWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UDragDropOperationWidget::InvokeReset(UUserWidget* BaseWidgetPtr)
{

}

void UDragDropOperationWidget::ResetToolUIByData(UBasicUnit* BasicUnitPtr)
{
	if (BasicUnitPtr)
	{
		switch (BasicUnitPtr->GetSceneToolsType())
		{
		case ESceneToolsType::kWeapon:
		{
			WeaponUnitSPtr = Cast<UWeaponUnit>(BasicUnitPtr);
			SetItemType(WeaponUnitSPtr);
		}
		break;
		case ESceneToolsType::kTool:
		{
			ToolUnitSPtr = Cast<UToolUnit>(BasicUnitPtr);
			SetItemType(ToolUnitSPtr);
		}
		break;
		case ESceneToolsType::kActiveSkill:
		{
			SkillUnitSPtr = Cast<USkillUnit>(BasicUnitPtr);
			SetItemType(SkillUnitSPtr);
		}
		break;
		case ESceneToolsType::kConsumables:
		{
			ConsumableUnitSPtr = Cast<UConsumableUnit>(BasicUnitPtr);
			SetItemType(ConsumableUnitSPtr);
		}
		break;
		}
	}
}

void UDragDropOperationWidget::EnableIcon(bool bIsEnable)
{

}

void UDragDropOperationWidget::SetNum(int32 NewNum)
{
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(TEXT("Number")));
	if (!NumTextPtr)
	{
		return;
	}
	if (NewNum > 0)
	{
		const auto NumStr = FString::Printf(TEXT("%d"), NewNum);

		NumTextPtr->SetText(FText::FromString(NumStr));
	}
	else
	{
		NumTextPtr->SetText(FText::FromString(TEXT("")));
	}
}

void UDragDropOperationWidget::SetItemType(UBasicUnit* ToolSPtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Texture")));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(ToolSPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr, ToolSPtr]()
			{
				ImagePtr->SetBrushFromTexture(ToolSPtr->GetIcon().Get());
			}));
	}
}

void UDragDropOperationWidget::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("SizeBox")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}
