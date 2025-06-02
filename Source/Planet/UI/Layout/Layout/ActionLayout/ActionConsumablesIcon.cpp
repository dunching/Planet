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
#include "BackpackIcon.h"

#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Skill_Base.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Consumable.h"
#include "Skill_Active_Base.h"

#include "Skill_Consumable_Generic.h"

struct FActionConsumablesIcon : public TStructVariable<FActionConsumablesIcon>
{
	const FName Percent = TEXT("Percent");

	const FName Content = TEXT("Content");

	const FName Number = TEXT("Number");

	const FName StateOverlap = TEXT("StateOverlap");

	const FName Icon = TEXT("Icon");

	const FName CooldownProgress = TEXT("CooldownProgress");

	const FName CooldownText = TEXT("CooldownText");

	const FName CanRelease = TEXT("CanRelease");

	const FName WaitInputPercent = TEXT("WaitInputPercent");
};

UActionConsumablesIcon::UActionConsumablesIcon(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void UActionConsumablesIcon::InvokeReset(
	UUserWidget* BaseWidgetPtr
	)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->ProxyPtr);
		}
	}
}

void UActionConsumablesIcon::ResetToolUIByData(
	const TSharedPtr<FBasicProxy>& BasicProxyPtr
	)
{
	bIsReady_Previous = false;

	ProxyPtr = nullptr;

	if (BasicProxyPtr)
	{
		if (
			BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables)
		)
		{
			ProxyPtr = DynamicCastSharedPtr<FConsumableProxy>(BasicProxyPtr);

			if (ProxyPtr && ProxyPtr->GetNum() > 0)
			{
				OnValueChangedDelegateHandle =
					ProxyPtr->CallbackContainerHelper.AddOnValueChanged(
					                                                    std::bind(
						                                                     &ThisClass::SetNum,
						                                                     this,
						                                                     std::placeholders::_1
						                                                    )
					                                                   );

				SetLevel();
				SetItemType();
				SetNum(ProxyPtr->GetNum());
				SetCanRelease(true);
				SetRemainingCooldown(true, 0.f, 0.f);
				SetInputRemainPercent(false, 0.f);

				return;
			}
		}
	}

	// 不放置消耗品的情况
	OnValueChangedDelegateHandle.Reset();
	SetLevel();
	SetItemType();
	SetNum(0);
	SetCanRelease(true);
	SetRemainingCooldown(true, 0.f, 0.f);
	SetInputRemainPercent(false, 0.f);
}

void UActionConsumablesIcon::EnableIcon(
	bool bIsEnable
	)
{
}

void UActionConsumablesIcon::UpdateState()
{
	if (ProxyPtr)
	{
		float RemainingCooldown = 0.f;
		float RemainingCooldownPercent = 0.f;

		auto bCooldownIsReady = ProxyPtr->GetRemainingCooldown(RemainingCooldown, RemainingCooldownPercent);
		SetRemainingCooldown(bCooldownIsReady, RemainingCooldown, RemainingCooldownPercent);

		SetCanRelease(bCooldownIsReady && (ProxyPtr->GetNum() > 0));
	}
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
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionConsumablesIcon::Get().StateOverlap));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionConsumablesIcon::Get().StateOverlap));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	{
		auto UIPtr = Cast<UImage>(GetWidgetFromName(FActionConsumablesIcon::Get().CooldownProgress));
		if (!UIPtr)
		{
			return;
		}
		auto MIDPtr = UIPtr->GetDynamicMaterial();
		if (MIDPtr)
		{
			MIDPtr->SetScalarParameterValue(FActionConsumablesIcon::Get().Percent, Percent);
		}
	}
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FActionConsumablesIcon::Get().CooldownText));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%.1lf"), RemainingTime)));
		}
	}
}

void UActionConsumablesIcon::SetCanRelease(
	bool bIsReady_In
	)
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
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionConsumablesIcon::Get().CanRelease));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionConsumablesIcon::Get().CanRelease));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UActionConsumablesIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FActionConsumablesIcon::Get().Icon));
	if (ImagePtr)
	{
		if (ProxyPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			AsyncLoadText(ProxyPtr->GetIcon(), ImagePtr);
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UActionConsumablesIcon::SetNum(
	int32 NewNum
	)
{
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(FActionConsumablesIcon::Get().Number));
	if (!NumTextPtr)
	{
		return;
	}
	if (NewNum > 0)
	{
		const auto NumStr = FString::Printf(TEXT("%d"), NewNum);

		NumTextPtr->SetText(FText::FromString(NumStr));
	}
	else
	{
		NumTextPtr->SetText(FText::FromString(TEXT("")));
	}
}

void UActionConsumablesIcon::SetInputRemainPercent(
	bool bIsAcceptInput,
	float Percent
	)
{
	auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(FActionConsumablesIcon::Get().WaitInputPercent));
	if (UIPtr)
	{
		UIPtr->SetVisibility(bIsAcceptInput ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		UIPtr->SetPercent(Percent);
	}
}

void UActionConsumablesIcon::SetDurationPercent(
	bool bIsHaveDuration,
	float Percent
	)
{
}

void UActionConsumablesIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

void UActionConsumablesIcon::NativeDestruct()
{
	if (OnValueChangedDelegateHandle)
	{
		OnValueChangedDelegateHandle->UnBindCallback();
	}

	Super::NativeDestruct();
}
