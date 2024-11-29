
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
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "ItemProxy.h"
#include "CharacterBase.h"
#include "PlanetControllerInterface.h"
#include "GroupMnaggerComponent.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAttibutes.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GroupSharedInfo.h"
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

void UTeamMateInfo::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)
{
	auto WidgetSwitcherPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FTeamMateInfo::Get().WidgetSwitcher));
	if (WidgetSwitcherPtr)
	{
		if (BasicUnitPtr && BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Character))
		{
			WidgetSwitcherPtr->SetActiveWidgetIndex(0);

			GroupMateUnitPtr = DynamicCastSharedPtr<FCharacterProxy>(BasicUnitPtr);
			{
				auto UIPtr = Cast<UImage>(GetWidgetFromName(FTeamMateInfo::Get().Icon));
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
				auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FTeamMateInfo::Get().Text));
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
		else
		{
			WidgetSwitcherPtr->SetActiveWidgetIndex(1);
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
	auto GMCPtr = PCPtr->GetGroupSharedInfo();
	GMCPtr->GetTeamMatesHelperComponent()->AddCharacterToTeam(GroupMateUnitPtr, Index);
}
