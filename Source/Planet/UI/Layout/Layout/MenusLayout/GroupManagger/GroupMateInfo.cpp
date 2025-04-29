
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
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "ItemProxy_Minimal.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAttibutes.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "ItemProxy_Character.h"

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
		auto DragWidgetPtr = CreateWidget<UItemProxyDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(GroupMateProxyPtr);

			auto WidgetDragPtr = Cast<UItemProxyDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemProxyDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = GroupMateProxyPtr;

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
			ResetToolUIByData(NewPtr->GroupMateProxyPtr);
		}
	}
}

void UGroupMateInfo::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)
{
	if (BasicProxyPtr && BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		GroupMateProxyPtr = DynamicCastSharedPtr<FCharacterProxy>(BasicProxyPtr);
		{
			auto ImagePtr = Cast<UImage>(GetWidgetFromName(FGroupMateInfo::Get().Icon));
			if (ImagePtr)
			{
			AsyncLoadText(GroupMateProxyPtr->GetIcon(),ImagePtr );
			}
		}
		{
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FGroupMateInfo::Get().Text));
			if (UIPtr)
			{
				auto CharacterAttributesSPtr =
					GroupMateProxyPtr->CharacterAttributesSPtr;

				if (GroupMateProxyPtr->Name.IsEmpty())
				{
					UIPtr->SetText(
						FText::FromString(FString::Printf(TEXT("%s(%d)"),
							*GroupMateProxyPtr->Title,
							GroupMateProxyPtr->Level))
					);
				}
				else
				{
					UIPtr->SetText(
						FText::FromString(FString::Printf(TEXT("%s %s(%d)"),
							*GroupMateProxyPtr->Title,
							*GroupMateProxyPtr->Name,
							GroupMateProxyPtr->Level))
					);
				}
			}
		}
	}
}

void UGroupMateInfo::EnableIcon(bool bIsEnable)
{

}
