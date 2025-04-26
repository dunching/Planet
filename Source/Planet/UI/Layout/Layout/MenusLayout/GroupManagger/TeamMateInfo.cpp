
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
#include "Components/WidgetSwitcher.h"


#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "ItemProxy_Minimal.h"
#include "CharacterBase.h"
#include "PlanetControllerInterface.h"
#include "TeamMatesHelperComponent.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAttibutes.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GroupManagger.h"
#include "ItemProxy_Character.h"

struct FTeamMateInfo : public TStructVariable<FTeamMateInfo>
{
	const FName Content = TEXT("Content");

	const FName Icon = TEXT("Icon");

	const FName Text = TEXT("Text");

	const FName WidgetSwitcher = TEXT("WidgetSwitcher");
};

void UTeamMateInfo::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InvokeReset(Cast<ThisClass>(ListItemObject));
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

	if (InOperation->IsA(UItemProxyDragDropOperation::StaticClass()))
	{
		auto WidgetDragPtr = Cast<UItemProxyDragDropOperation>(InOperation);
		if (WidgetDragPtr)
		{
			auto PrevProxySPtr = GroupMateProxyPtr;
			ResetToolUIByData(WidgetDragPtr->SceneToolSPtr);
			if (GroupMateProxyPtr && !bPaseInvokeOnResetProxyEvent)
			{
				OnDroped.ExcuteCallback(this, PrevProxySPtr, GroupMateProxyPtr);
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
			ResetToolUIByData(NewPtr->GroupMateProxyPtr);
		}
	}
}

void UTeamMateInfo::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)
{
	auto WidgetSwitcherPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FTeamMateInfo::Get().WidgetSwitcher));
	if (WidgetSwitcherPtr)
	{
		if (BasicProxyPtr && BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Character))
		{
			WidgetSwitcherPtr->SetActiveWidgetIndex(0);

			GroupMateProxyPtr = DynamicCastSharedPtr<FCharacterProxy>(BasicProxyPtr);
			{
				auto UIPtr = Cast<UImage>(GetWidgetFromName(FTeamMateInfo::Get().Icon));
				if (UIPtr)
				{
					FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
					AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(GroupMateProxyPtr->GetIcon().ToSoftObjectPath(), [this, UIPtr]()
						{
							UIPtr->SetBrushFromTexture(GroupMateProxyPtr->GetIcon().Get());
						}));
				}
			}
			{
				auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FTeamMateInfo::Get().Text));
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
		else
		{
			GroupMateProxyPtr = nullptr;
			WidgetSwitcherPtr->SetActiveWidgetIndex(1);
		}
	}
}

void UTeamMateInfo::EnableIcon(bool bIsEnable)
{

}

void UTeamMateInfo::SynMember2Config(int32 Index )
{
	auto PCPtr = Cast<IPlanetControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}
	auto GMCPtr = PCPtr->GetGroupManagger();
	GMCPtr->GetTeamMatesHelperComponent()->UpdateTeammateConfig(GroupMateProxyPtr, Index);
}
