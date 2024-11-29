
#include "BackpackIconWrapper.h"

#include <Kismet/GameplayStatics.h>
#include "Containers/UnrealString.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "ItemProxy.h"
#include "BackpackToolIcon.h"
#include "BackpackConsumableIcon.h"
#include "GameplayTagsLibrary.h"
#include "BackpackSkillIcon.h"
#include "BackpackWeaponIcon.h"

namespace BackpackIconWrapper
{
	const FName Border = TEXT("Border");
}

UBackpackIconWrapper::UBackpackIconWrapper(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UBackpackIconWrapper::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InvokeReset(Cast<ThisClass>(ListItemObject));
}

void UBackpackIconWrapper::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			OnDragIconDelegate = NewPtr->OnDragIconDelegate;
			ResetToolUIByData(NewPtr->TargetBasicUnitPtr);
		}
	}
}

void UBackpackIconWrapper::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)
{
	if (BasicUnitPtr)
	{
		if (BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Tool))
		{
			auto UIPtr = Cast<UBorder>(GetWidgetFromName(BackpackIconWrapper::Border));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}

			auto WidgetPtr = CreateWidget<UBackpackToolIcon>(this, ToolUnitClass);
			if (WidgetPtr)
			{
				UIPtr->AddChild(WidgetPtr);

				WidgetPtr->OnDragDelegate = OnDragIconDelegate;
				WidgetPtr->ResetToolUIByData(BasicUnitPtr);
			}
		}
		else if (BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Consumables))
		{
			auto UIPtr = Cast<UBorder>(GetWidgetFromName(BackpackIconWrapper::Border));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}

			auto WidgetPtr = CreateWidget<UBackpackConsumableIcon>(this, ConsumableUnitClass);
			if (WidgetPtr)
			{
				UIPtr->AddChild(WidgetPtr);

				WidgetPtr->OnDragDelegate = OnDragIconDelegate;
				WidgetPtr->ResetToolUIByData(BasicUnitPtr);
			}
		}
		else if (BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Skill))
		{
			auto UIPtr = Cast<UBorder>(GetWidgetFromName(BackpackIconWrapper::Border));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}

			auto WidgetPtr = CreateWidget<UBackpackSkillIcon>(this, BackpackSkillIconClass);
			if (WidgetPtr)
			{
				UIPtr->AddChild(WidgetPtr);

				WidgetPtr->OnDragDelegate = OnDragIconDelegate;
				WidgetPtr->ResetToolUIByData(BasicUnitPtr);
			}
		}
		else if (BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Weapon))
		{
			auto UIPtr = Cast<UBorder>(GetWidgetFromName(BackpackIconWrapper::Border));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}

			auto WidgetPtr = CreateWidget<UBackpackWeaponIcon>(this, BackpackWeaponIconClass);
			if (WidgetPtr)
			{
				UIPtr->AddChild(WidgetPtr);

				WidgetPtr->OnDragDelegate = OnDragIconDelegate;
				WidgetPtr->ResetToolUIByData(BasicUnitPtr);
			}
		}
	}
}

void UBackpackIconWrapper::EnableIcon(bool bIsEnable)
{

}
