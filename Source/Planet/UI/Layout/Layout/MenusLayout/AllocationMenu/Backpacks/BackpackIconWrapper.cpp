
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
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "ItemProxy_Minimal.h"
#include "BackpackToolIcon.h"
#include "BackpackConsumableIcon.h"
#include "BackpackMaterialIcon.h"
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
			AllocationSkillsMenuPtr = NewPtr->AllocationSkillsMenuPtr;
			ResetToolUIByData(NewPtr->TargetBasicProxyPtr);
		}
	}
}

void UBackpackIconWrapper::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)
{
	if (BasicProxyPtr)
	{
		TargetBasicProxyPtr = BasicProxyPtr;
		
		if (BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
		{
			auto UIPtr = Cast<UBorder>(GetWidgetFromName(BackpackIconWrapper::Border));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}

			auto WidgetPtr = CreateWidget<UBackpackToolIcon>(this, ToolProxyClass);
			if (WidgetPtr)
			{
				UIPtr->AddChild(WidgetPtr);

				WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
				WidgetPtr->ResetToolUIByData(BasicProxyPtr);
			}
		}
		else if (BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
		{
			auto UIPtr = Cast<UBorder>(GetWidgetFromName(BackpackIconWrapper::Border));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}

			auto WidgetPtr = CreateWidget<UBackpackConsumableIcon>(this, ConsumableProxyClass);
			if (WidgetPtr)
			{
				UIPtr->AddChild(WidgetPtr);

				WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
				WidgetPtr->ResetToolUIByData(BasicProxyPtr);
			}
		}
		else if (BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill))
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

				WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
				WidgetPtr->ResetToolUIByData(BasicProxyPtr);
			}
		}
		else if (BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
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

				WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
				WidgetPtr->ResetToolUIByData(BasicProxyPtr);
			}
		}
		else if (BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Material))
		{
			auto UIPtr = Cast<UBorder>(GetWidgetFromName(BackpackIconWrapper::Border));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}

			auto WidgetPtr = CreateWidget<UBackpackMaterialIcon>(this, BackpackMaterialIconClass);
			if (WidgetPtr)
			{
				UIPtr->AddChild(WidgetPtr);

				WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
				WidgetPtr->ResetToolUIByData(BasicProxyPtr);
			}
		}
	}
}

void UBackpackIconWrapper::EnableIcon(bool bIsEnable)
{

}
