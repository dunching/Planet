
#include "ActionConsumablesIcon.h"

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
#include "InteractiveSkillComponent.h"
#include "Skill_Base.h"
#include "GameplayTagsSubSystem.h"
#include "Skill_Active_Base.h"
#include "InteractiveConsumablesComponent.h"

struct FActionSkillsIcon : public TStructVariable<FActionSkillsIcon>
{
	const FName Percent = TEXT("Percent");

	const FName Content = TEXT("Content");

	const FName StateOverlap = TEXT("StateOverlap");

	const FName Icon = TEXT("Icon");

	const FName CooldownProgress = TEXT("CooldownProgress");

	const FName CooldownText = TEXT("CooldownText");

	const FName CanRelease = TEXT("CanRelease");

	const FName WaitInputPercent = TEXT("WaitInputPercent");
};

UActionConsumablesIcon::UActionConsumablesIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UActionConsumablesIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->UnitPtr);
		}
	}
}

void UActionConsumablesIcon::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	bIsReady_Previous = false;

	UnitPtr = nullptr;

	if (BasicUnitPtr)
	{
		if (
			BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables)
			)
		{
			UnitPtr = Cast<UConsumableUnit>(BasicUnitPtr);
		}
	}

	SetLevel();
	SetItemType();
	SetCanRelease(true);
	SetRemainingCooldown(true, 0.f, 0.f);
	SetInputRemainPercent(false, 0.f);
}

void UActionConsumablesIcon::EnableIcon(bool bIsEnable)
{

}

void UActionConsumablesIcon::UpdateState()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetInteractiveConsumablesComponent();
	const auto ConsumableSocketInfoSPtr = EICPtr->FindConsumable(IconSocket);
	if (!ConsumableSocketInfoSPtr)
	{
		return;
	}

	const auto ConsumableUnitType = ConsumableSocketInfoSPtr->UnitPtr->GetUnitType();
	auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
}

void UActionConsumablesIcon::SetLevel()
{
}

void UActionConsumablesIcon::SetRemainingCooldown(
	bool bCooldownIsReady,
	float RemainingTime,
	float Percent
)
{
	if (bCooldownIsReady)
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionSkillsIcon::Get().StateOverlap));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionSkillsIcon::Get().StateOverlap));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	{
		auto UIPtr = Cast<UImage>(GetWidgetFromName(FActionSkillsIcon::Get().CooldownProgress));
		if (!UIPtr)
		{
			return;
		}
		auto MIDPtr = UIPtr->GetDynamicMaterial();
		if (MIDPtr)
		{
			MIDPtr->SetScalarParameterValue(FActionSkillsIcon::Get().Percent, Percent);
		}
	}
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FActionSkillsIcon::Get().CooldownText));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%.1lf"), RemainingTime)));
		}
	}
}

void UActionConsumablesIcon::SetCanRelease(bool bIsReady_In)
{
	if (bIsReady_In)
	{
		if (bIsReady_Previous)
		{
			return;
		}
		else
		{
			PlaySkillIsReady();
		}
	}

	bIsReady_Previous = bIsReady_In;

	if (bIsReady_In)
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionSkillsIcon::Get().CanRelease));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionSkillsIcon::Get().CanRelease));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UActionConsumablesIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FActionSkillsIcon::Get().Icon));
	if (ImagePtr)
	{
		if (UnitPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(UnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
				{
					ImagePtr->SetBrushFromTexture(UnitPtr->GetIcon().Get());
				}));
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UActionConsumablesIcon::SetInputRemainPercent(bool bIsAcceptInput, float Percent)
{
	auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(FActionSkillsIcon::Get().WaitInputPercent));
	if (UIPtr)
	{
		UIPtr->SetVisibility(bIsAcceptInput ?  ESlateVisibility::Visible : ESlateVisibility::Hidden);
		UIPtr->SetPercent(Percent);
	}
}

void UActionConsumablesIcon::SetDurationPercent(bool bIsHaveDuration, float Percent)
{
}

void UActionConsumablesIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}
