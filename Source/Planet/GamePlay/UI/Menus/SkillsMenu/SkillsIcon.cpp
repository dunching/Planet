
#include "SkillsIcon.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CanvasPanel.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetStringLibrary.h"

#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "BackpackIcon.h"
#include <CacheAssetManager.h>
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "Skill_Base.h"

const FName Content = TEXT("Content");

const FName HighlightBorder = TEXT("HighlightBorder");

const FName StateOverlap = TEXT("StateOverlap");

const FName Icon = TEXT("Icon");

const FName CooldownProgress = TEXT("CooldownProgress");

const FName CooldownText = TEXT("CooldownText");

const FName DisableMask = TEXT("DisableMask");

USkillsIcon::USkillsIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void USkillsIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			bIsInBackpakc = NewPtr->bIsInBackpakc;
			ResetToolUIByData(NewPtr->SkillUnitPtr);
		}
	}
}

void USkillsIcon::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	bIsReady_Previous = false;

	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kSkill)
	{
		SkillUnitPtr = Cast<USkillUnit>(BasicUnitPtr);

		if (SkillUnitPtr && (bIsInBackpakc || SkillUnitPtr->SkillType == SkillType))
		{

			SetLevel(SkillUnitPtr->Level);
			SetItemType();

			{
				auto ImagePtr = Cast<UImage>(GetWidgetFromName(HighlightBorder));
				if (ImagePtr)
				{
					ImagePtr->SetVisibility(ESlateVisibility::Hidden);
				}
			}
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(Content));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Visible);
				}
			}
			{
				auto UIPtr = Cast<UOverlay>(GetWidgetFromName(StateOverlap));
				if (UIPtr)
				{
					UIPtr->SetVisibility(ESlateVisibility::Hidden);
				}
			}
			return;
		}
	}
	{
		auto ImagePtr = Cast<UImage>(GetWidgetFromName(HighlightBorder));
		if (ImagePtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);
		}
	}
	{
		auto BorderPtr = Cast<UBorder>(GetWidgetFromName(Content));
		if (BorderPtr)
		{
			BorderPtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	{
		auto UIPtr = Cast<UOverlay>(GetWidgetFromName(StateOverlap));
		if (UIPtr)
		{
			UIPtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void USkillsIcon::SetLevel(int32 NewNum)
{
}

void USkillsIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(Icon));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(SkillUnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
			{
				ImagePtr->SetBrushFromTexture(SkillUnitPtr->GetIcon().Get());
			});
	}
}

void USkillsIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

FReply USkillsIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (bIsInBackpakc)
		{
			Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

			return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
		}
		else
		{
			return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
		}
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		ResetToolUIByData(nullptr);
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool USkillsIcon::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (InOperation->IsA(UItemsDragDropOperation::StaticClass()))
	{
 		auto WidgetDragPtr = Cast<UItemsDragDropOperation>(InOperation);
 		if (WidgetDragPtr)
		{
			if (WidgetDragPtr->bIsInBackpakc)
			{
				ResetToolUIByData(WidgetDragPtr->SceneToolSPtr);
			}
 		}
	}

	return true;
}

void USkillsIcon::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	auto BaseItemClassPtr = UAssetRefMap::GetInstance()->DragDropOperationWidgetClass;
	
	if (BaseItemClassPtr)
	{
		auto DragWidgetPtr = CreateWidget<UDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(SkillUnitPtr);

			auto WidgetDragPtr = Cast<UItemsDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemsDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = SkillUnitPtr;
				WidgetDragPtr->bIsInBackpakc = bIsInBackpakc;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}
