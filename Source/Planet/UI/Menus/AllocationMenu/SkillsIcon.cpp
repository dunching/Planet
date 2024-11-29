
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
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Skill_Base.h"
#include "GameplayTagsLibrary.h"

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

void USkillsIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicUnitPtr)
{
	if (InBasicUnitPtr == BasicUnitPtr)
	{
		return;
	}

	auto PreviousUnitPtr = BasicUnitPtr;
	TSharedPtr<FBasicProxy> NewUnitPtr = nullptr;
	if (InBasicUnitPtr && InBasicUnitPtr->GetUnitType().MatchesTag(UnitType))
	{
		NewUnitPtr = InBasicUnitPtr;
	}
	else
	{
		if (UnitType.MatchesTag(UGameplayTagsLibrary::Unit_Skill_Passve))
		{
			auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
			if (!CharacterPtr)
			{
				return;
			}

			// 装备了可以在“被动”槽上备注“主动技能”的主动技能
			if (CharacterPtr->GetProxyProcessComponent()->FindActiveSkillByType(
				UGameplayTagsLibrary::Unit_Skill_Active_Switch
			).IsValid())
			{
				NewUnitPtr = InBasicUnitPtr;
			}
		}

	}

	if (!bPaseInvokeOnResetUnitEvent)
	{
		OnResetProxy.ExcuteCallback(PreviousUnitPtr, NewUnitPtr);
	}

	BasicUnitPtr = NewUnitPtr;

	if (!bPaseInvokeOnResetUnitEvent)
	{
		OnResetData(this);
	}

	SetItemType();

	SetLevel();
}

void USkillsIcon::EnableIcon(bool bIsEnable)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(SkillsIcon::Enable));
	if (ImagePtr)
	{
		ImagePtr->SetVisibility(bIsEnable ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
}

void USkillsIcon::SetLevel()
{
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

void USkillsIcon::OnDragIcon(bool bIsDragging, const TSharedPtr<FBasicProxy>& UnitPtr)
{
	if (bIsDragging)
	{
		if (UnitPtr && UnitPtr->GetUnitType().MatchesTag(UnitType))
		{
			EnableIcon(true);
		}
		else
		{
			if (
				UnitType.MatchesTag(UGameplayTagsLibrary::Unit_Skill_Passve) &&
				UnitPtr->GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Skill_Active)
				)
			{
				auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
				if (!CharacterPtr)
				{
					return;
				}

				// 装备了可以在“被动”槽上备注“主动技能”的主动技能
				if (CharacterPtr->GetProxyProcessComponent()->FindActiveSkillByType(
					UGameplayTagsLibrary::Unit_Skill_Active_Switch
				).IsValid())
				{
					EnableIcon(true);

					return;
				}
			}

			EnableIcon(false);
		}
	}
	else
	{
		EnableIcon(true);
	}
}
