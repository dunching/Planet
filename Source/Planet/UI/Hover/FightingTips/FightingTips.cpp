
#include "FightingTips.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>
#include <Components/VerticalBox.h>

#include "CharacterBase.h"
#include "FightingTipsItem.h"
#include "Planet.h"

namespace FightingTips
{
	const FName VerticalBox = TEXT("VerticalBox");
}

void UFightingTips::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition)
	);

	ResetPosition(0.f);

	SetVisibility(ESlateVisibility::Hidden);
}

void UFightingTips::NativeDestruct()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	Super::NativeDestruct();
}

void UFightingTips::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UFightingTips::ProcessGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
	const auto& Ref = GAEvent.Data;
	if (Ref.TargetCharacterPtr.IsValid())
	{
		TargetCharacterPtr = Ref.TargetCharacterPtr.Get();

		auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(FightingTips::VerticalBox));
		if (!PanelPtr)
		{
			return;
		}
		PanelPtr->ClearChildren();

		if (Ref.TrueDamage > 0)
		{
			auto UIPtr = CreateWidget<UFightingTipsItem>(GetWorldImp(), FightingTipsItemClass);
			UIPtr->ProcessGAEVent(UFightingTipsItem::EType::kTrueDamage, GAEvent);
			PanelPtr->AddChild(UIPtr);
		}

		if (Ref.ElementSet.Num() > 0)
		{
			for (const auto& Iter : Ref.ElementSet)
			{
				auto UIPtr = CreateWidget<UFightingTipsItem>(GetWorldImp(), FightingTipsItemClass);
				switch (Iter.Get<0>())
				{
				case EWuXingType::kGold:
				{
					UIPtr->ProcessGAEVent(UFightingTipsItem::EType::kGold, GAEvent);
				}
				break;
				case EWuXingType::kWood:
				{
					UIPtr->ProcessGAEVent(UFightingTipsItem::EType::kWood, GAEvent);
				}
				break;
				case EWuXingType::kWater:
				{
					UIPtr->ProcessGAEVent(UFightingTipsItem::EType::kWater, GAEvent);
				}
				break;
				case EWuXingType::kFire:
				{
					UIPtr->ProcessGAEVent(UFightingTipsItem::EType::kFire, GAEvent);
				}
				break;
				case EWuXingType::kSoil:
				{
					UIPtr->ProcessGAEVent(UFightingTipsItem::EType::kSoil, GAEvent);
				}
				break;
				}
				PanelPtr->AddChild(UIPtr);
			}
		}
		else if (Ref.BaseDamage > 0)
		{
			auto UIPtr = CreateWidget<UFightingTipsItem>(GetWorldImp(), FightingTipsItemClass);
			UIPtr->ProcessGAEVent(UFightingTipsItem::EType::kBaseDamage, GAEvent);
			PanelPtr->AddChild(UIPtr);
		}

		if (Ref.HP > 0)
		{
			auto UIPtr = CreateWidget<UFightingTipsItem>(GetWorldImp(), FightingTipsItemClass);
			UIPtr->ProcessGAEVent(UFightingTipsItem::EType::kTreatment, GAEvent);
			PanelPtr->AddChild(UIPtr);
		}
	}
	else
	{
		RemoveFromParent();
	}
}

void UFightingTips::PlayAnimationFinished()
{
	RemoveFromParent();
}

bool UFightingTips::ResetPosition(float InDeltaTime)
{
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(
		UGameplayStatics::GetPlayerController(this, 0),
		TargetCharacterPtr->GetActorLocation(),
		ScreenPosition
	);

	if (SizeBox.IsNearlyZero())
	{
		SizeBox = GetDesiredSize();

		if (SizeBox.IsNearlyZero())
		{
			return true;
		}
		else
		{
			PlayMyAnimation();
			SetVisibility(ESlateVisibility::Visible);
		}
	}

	const auto Scale = UWidgetLayoutLibrary::GetViewportScale(this);

	const auto TempWidgetSize = SizeBox * Scale;

	ScreenPosition -= TempWidgetSize / 2;

	SetPositionInViewport(ScreenPosition);

	return true;
}
