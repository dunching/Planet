
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

namespace SkillsIcon
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

	const FName Icon = TEXT("Icon");
}

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
			OnResetUnit = NewPtr->OnResetUnit;
			OnDragDelegate = NewPtr->OnDragDelegate;
			bIsInBackpakc = NewPtr->bIsInBackpakc;
			SkillType = NewPtr->SkillType;
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
		}
		else
		{
			SkillUnitPtr = nullptr;
		}
	}

	OnResetUnit.ExcuteCallback(SkillUnitPtr);
	SetLevel();
	SetItemType();
}

void USkillsIcon::EnableIcon(bool bIsEnable)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(SkillsIcon::Enable));
	if (ImagePtr)
	{
		ImagePtr->SetVisibility(bIsEnable ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
}

void USkillsIcon::OnDragSkillIcon(bool bIsDragging, USkillUnit* InSkillUnitPtr)
{
	if (bIsDragging)
	{
		if (InSkillUnitPtr && InSkillUnitPtr->SkillType != SkillType)
		{
			EnableIcon(false);
		}
	}
	else
	{
		EnableIcon(true);
	}
}

void USkillsIcon::OnDragWeaponIcon(bool bIsDragging, UWeaponUnit* WeaponUnitPtr)
{
	if (bIsDragging)
	{
		if (WeaponUnitPtr)
		{
			EnableIcon(false);
		}
	}
	else
	{
		EnableIcon(true);
	}
}

void USkillsIcon::SetLevel()
{
}

void USkillsIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(SkillsIcon::Icon));
	if (ImagePtr)
	{
		if (SkillUnitPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(SkillUnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
				{
					ImagePtr->SetBrushFromTexture(SkillUnitPtr->GetIcon().Get());
				});
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void USkillsIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

void USkillsIcon::NativeDestruct()
{
	Super::NativeDestruct();
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
			ResetToolUIByData(WidgetDragPtr->SceneToolSPtr);

			WidgetDragPtr->OnDrop.Broadcast(WidgetDragPtr);
 		}
	}

	return true;
}

void USkillsIcon::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	OnDragDelegate.ExcuteCallback(false, nullptr);
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
				WidgetDragPtr->OnDrop.AddDynamic(this, &ThisClass::OnDroped);

				OutOperation = WidgetDragPtr;

				OnDragDelegate.ExcuteCallback(true, SkillUnitPtr);
			}
		}
	}
}

void USkillsIcon::OnDroped(UDragDropOperation* Operation)
{
	OnDragDelegate.ExcuteCallback(false, nullptr);
}
