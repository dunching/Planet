
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

#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "ItemProxy.h"
#include "GameplayTagsSubSystem.h"
#include "CharacterAttibutes.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"

struct FGroupMateInfo : public TStructVariable<FGroupMateInfo>
{
	const FName Icon = TEXT("Icon");

	const FName Text = TEXT("Text");
};

void UGroupMateInfo::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InvokeReset(Cast<ThisClass>(ListItemObject));
}

FReply UGroupMateInfo::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UGroupMateInfo::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	auto BaseItemClassPtr = UAssetRefMap::GetInstance()->DragDropOperationWidgetClass;

	if (BaseItemClassPtr)
	{
		auto DragWidgetPtr = CreateWidget<UDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(GroupMateUnitPtr);

			auto WidgetDragPtr = Cast<UItemsDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemsDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = GroupMateUnitPtr;

				OutOperation = WidgetDragPtr;
			}
		}
	}
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

void UGroupMateInfo::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Character))
	{
		GroupMateUnitPtr = DynamicCastSharedPtr<FCharacterProxy>(BasicUnitPtr);
		{
			auto UIPtr = Cast<UImage>(GetWidgetFromName(FGroupMateInfo::Get().Icon));
			if (UIPtr)
			{
				FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
				AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(GroupMateUnitPtr->GetIcon().ToSoftObjectPath(), [this, UIPtr]()
					{
						UIPtr->SetBrushFromTexture(GroupMateUnitPtr->GetIcon().Get());
					}));
			}
		}
		{
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FGroupMateInfo::Get().Text));
			if (UIPtr)
			{
				auto CharacterAttributesSPtr =
					GroupMateUnitPtr->CharacterAttributesSPtr;

				if (CharacterAttributesSPtr->Name.IsEmpty())
				{
					UIPtr->SetText(
						FText::FromString(FString::Printf(TEXT("%s(%d)"),
							*CharacterAttributesSPtr->Title,
							CharacterAttributesSPtr->Level))
					);
				}
				else
				{
					UIPtr->SetText(
						FText::FromString(FString::Printf(TEXT("%s %s(%d)"),
							*CharacterAttributesSPtr->Title,
							*CharacterAttributesSPtr->Name,
							CharacterAttributesSPtr->Level))
					);
				}
			}
		}
	}
}

void UGroupMateInfo::EnableIcon(bool bIsEnable)
{

}
