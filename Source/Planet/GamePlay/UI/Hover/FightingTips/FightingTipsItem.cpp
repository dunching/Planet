
#include "FightingTipsItem.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>
#include <Components/TextBlock.h>
#include <Components/Image.h>

#include "CharacterBase.h"

namespace FightingTipsItem
{
	const FName Icon = TEXT("Icon");

	const FName Icon_Disable = TEXT("Icon_Disable");
	
	const FName Icon_Treatment = TEXT("Icon_Treatment");

	const FName Text = TEXT("Text");

	const FName SizeBox = TEXT("SizeBox");
}

void UFightingTipsItem::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));
}

void UFightingTipsItem::NativeDestruct()
{
	Super::NativeDestruct();
}

void UFightingTipsItem::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UFightingTipsItem::ProcessGAEVent(UFightingTipsItem::EType Type, const FGameplayAbilityTargetData_GAEvent& GAEvent)
{
	if (GAEvent.TargetActorAry.IsValidIndex(0))
	{
		auto Lambda = [&](int32 Value) {
			// 
			if (GAEvent.Data.HitRate <= 0)
			{
				auto UIPtr = Cast<UImage>(GetWidgetFromName(FightingTipsItem::Icon_Disable));
				if (!UIPtr)
				{
					return;
				}
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
			else if (GAEvent.Data.CriticalHitRate >= 100)
			{
				auto UIPtr = Cast<USizeBox>(GetWidgetFromName(FightingTipsItem::SizeBox));
				if (!UIPtr)
				{
					return;
				}
				UIPtr->SetHeightOverride(Size.X * 1.5f);
				UIPtr->SetWidthOverride(Size.X * 1.5f);
			}

			{
				auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FightingTipsItem::Text));
				if (!UIPtr)
				{
					return;
				}
				UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%d"), Value)));
			}
			};

		switch (Type)
		{
		case UFightingTipsItem::EType::kBaseDamage:
		{
			Lambda(GAEvent.Data.BaseDamage);
		}
		break;
		case UFightingTipsItem::EType::kTrueDamage:
		{
			Lambda(GAEvent.Data.TrueDamage);
		}
		break;
		case UFightingTipsItem::EType::kGold:
		{
			for (const auto& Iter : GAEvent.Data.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kGold:
				{
					Lambda(Iter.Get<2>());
					bIsBreak = true;
				}
				break;
				}
				if (bIsBreak)
				{
					break;
				}
			}
		}
		break;
		case UFightingTipsItem::EType::kWood:
		{
			for (const auto& Iter : GAEvent.Data.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kWood:
				{
					Lambda(Iter.Get<2>());
					bIsBreak = true;
				}
				break;
				}
				if (bIsBreak)
				{
					break;
				}
			}
		}
		break;
		case UFightingTipsItem::EType::kWater:
		{
			for (const auto& Iter : GAEvent.Data.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kWater:
				{
					Lambda(Iter.Get<2>());
					bIsBreak = true;
				}
				break;
				}
				if (bIsBreak)
				{
					break;
				}
			}
		}
		break;
		case UFightingTipsItem::EType::kFire:
		{
			for (const auto& Iter : GAEvent.Data.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kFire:
				{
					Lambda(Iter.Get<2>());
					bIsBreak = true;
				}
				break;
				}
				if (bIsBreak)
				{
					break;
				}
			}
		}
		break;
		case UFightingTipsItem::EType::kSoil:
		{
			for (const auto& Iter : GAEvent.Data.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kSoil:
				{
					Lambda(Iter.Get<2>());
					bIsBreak = true;
				}
				break;
				}
				if (bIsBreak)
				{
					break;
				}
			}
		}
		break;
		case UFightingTipsItem::EType::kTreatment:
		{
			{
				auto UIPtr = Cast<UImage>(GetWidgetFromName(FightingTipsItem::Icon));
				if (!UIPtr)
				{
					return;
				}
				UIPtr->SetVisibility(ESlateVisibility::Collapsed);
			}
			{
				auto UIPtr = Cast<UImage>(GetWidgetFromName(FightingTipsItem::Icon_Treatment));
				if (!UIPtr)
				{
					return;
				}
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FightingTipsItem::Text));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetText(FText::FromString(FString::Printf(TEXT("+%d"), GAEvent.Data.TreatmentVolume)));
			UIPtr->SetColorAndOpacity(FSlateColor(FLinearColor(0.000000, 1.000000, 0.191129, 1.000000)));
		}
		break;
		}
	}
}