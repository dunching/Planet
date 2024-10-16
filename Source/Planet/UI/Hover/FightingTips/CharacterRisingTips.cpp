
#include "CharacterRisingTips.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>
#include <Components/VerticalBox.h>
#include <Components/TextBlock.h>
#include <Components/Image.h>
#include "Components/WidgetSwitcher.h"

#include "CharacterBase.h"
#include "Planet.h"
#include "GameOptions.h"
#include "TemplateHelper.h"
#include "TextSubSystem.h"
#include "TextCollect.h"

struct FCharacterRisingTips : public TStructVariable<FCharacterRisingTips>
{
	const FName VerticalBox = TEXT("VerticalBox");

	const FName Icon = TEXT("Icon");

	const FName Icon_Disable = TEXT("Icon_Disable");

	const FName Icon_Treatment = TEXT("Icon_Treatment");

	const FName Text = TEXT("Text");

	const FName SizeBox = TEXT("SizeBox");

	const FName WidgetSwitcher = TEXT("WidgetSwitcher");
};

void UCharacterRisingTips::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));
	SetAlignmentInViewport(FVector2D(.5f, .5f));

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition)
	);

	PlayMyAnimation(bIsCritical);

	ResetPosition(0.f);
}

void UCharacterRisingTips::NativeDestruct()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	Super::NativeDestruct();
}

void UCharacterRisingTips::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

bool UCharacterRisingTips::ProcessGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
	const auto& Ref = GAEvent.Data;
	if (Ref.TargetCharacterPtr.IsValid())
	{
		TargetCharacterPtr = Ref.TargetCharacterPtr.Get();

		if (Ref.ElementSet.Num() > 0)
		{
			for (const auto& Iter : Ref.ElementSet)
			{
				switch (Iter.Get<0>())
				{
				case EWuXingType::kGold:
				{
					ProcessGAEVentImp(EType::kGold, GAEvent);
				}
				break;
				case EWuXingType::kWood:
				{
					ProcessGAEVentImp(EType::kWood, GAEvent);
				}
				break;
				case EWuXingType::kWater:
				{
					ProcessGAEVentImp(EType::kWater, GAEvent);
				}
				break;
				case EWuXingType::kFire:
				{
					ProcessGAEVentImp(EType::kFire, GAEvent);
				}
				break;
				case EWuXingType::kSoil:
				{
					ProcessGAEVentImp(EType::kSoil, GAEvent);
				}
				break;
				}
			}
		}
		else if (Ref.BaseDamage > 0)
		{
			ProcessGAEVentImp(EType::kBaseDamage, GAEvent);
		}

#if UE_EDITOR || UE_CLIENT
		if (TargetCharacterPtr->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
		{
			if (
				Ref.DataModify.Contains(ECharacterPropertyType::HP) &&
				(Ref.DataModify[ECharacterPropertyType::HP].GetCurrentValue() > 0)
				)
			{
				ProcessGAEVentImp(EType::kTreatment, GAEvent);
			}
		}
		else if (TargetCharacterPtr->GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
		{
			const auto NPC_HP_Display_MoveToAttaclAreaOffset = UGameOptions::GetInstance()->NPC_HP_Display_MoveToAttaclAreaOffset;
			if (Ref.DataModify.Contains(ECharacterPropertyType::HP))
			{
				if (Ref.DataModify[ECharacterPropertyType::HP].GetCurrentValue() > NPC_HP_Display_MoveToAttaclAreaOffset)
				{
					ProcessGAEVentImp(EType::kTreatment, GAEvent);
				}
				else
				{
					return false;
				}
			}
		}
#endif

		return true;
	}

	return false;
}

void UCharacterRisingTips::PlayAnimationFinished()
{
	RemoveFromParent();
}

bool UCharacterRisingTips::ResetPosition(float InDeltaTime)
{
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(
		UGameplayStatics::GetPlayerController(this, 0),
		TargetCharacterPtr->GetActorLocation(),
		ScreenPosition
	);

	SetPositionInViewport(ScreenPosition);

	return true;
}

void UCharacterRisingTips::ProcessGAEVentImp(EType Type, const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
	const auto& Ref = GAEvent.Data;
	if (Ref.TargetCharacterPtr.IsValid())
	{
		auto Lambda = [&](int32 Value, const FLinearColor & Color)
			{
				if 
					(
						(Ref.CriticalHitRate >= 100) &&
						(Ref.HitRate >= 100)
					)
				{
					bIsCritical = true;
					{
						auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterRisingTips::Get().Text));
						if (!UIPtr)
						{
							return;
						}
						const auto Text = FText::FromString(
							FString::Printf(TEXT("%s:%d"), *UTextSubSystem::GetInstance()->GetText(TextCollect::Critical), Value)
						);
						UIPtr->SetText(Text);
						UIPtr->SetColorAndOpacity(FSlateColor(Color));
					}
				}
				else if (
					(Ref.HitRate >= 100)
					)
				{
					{
						auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterRisingTips::Get().Text));
						if (!UIPtr)
						{
							return;
						}
						UIPtr->SetText(FText::FromString(FString::Printf(TEXT("-%d"), Value)));
						UIPtr->SetColorAndOpacity(FSlateColor(Color));
					}
				}
				// 
				else if (Ref.HitRate <= 0)
				{
					{
						auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterRisingTips::Get().Text));
						if (!UIPtr)
						{
							return;
						}
						const auto Text = FText::FromString(
							FString::Printf(TEXT("%s:%d"), *UTextSubSystem::GetInstance()->GetText(TextCollect::Evade), Value)
						);
						UIPtr->SetText(Text);
						UIPtr->SetColorAndOpacity(FSlateColor(Color));
					}
				}
			};

		switch (Type)
		{
		case EType::kBaseDamage:
		{
			{
				auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FCharacterRisingTips::Get().WidgetSwitcher));
				if (!UIPtr)
				{
					return;
				}
				UIPtr->SetActiveWidgetIndex(kIcon_BaseDamage);
			}
			Lambda(Ref.BaseDamage, BaseDamageColor);
		}
		break;
		case EType::kTrueDamage:
		{
			Lambda(Ref.TrueDamage, BaseDamageColor);
		}
		break;
		case EType::kGold:
		{
			for (const auto& Iter : Ref.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kGold:
				{
					Lambda(Iter.Get<2>(), BaseDamageColor);
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
		case EType::kWood:
		{
			for (const auto& Iter : Ref.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kWood:
				{
					Lambda(Iter.Get<2>(), BaseDamageColor);
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
		case EType::kWater:
		{
			for (const auto& Iter : Ref.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kWater:
				{
					Lambda(Iter.Get<2>(), BaseDamageColor);
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
		case EType::kFire:
		{
			for (const auto& Iter : Ref.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kFire:
				{
					Lambda(Iter.Get<2>(), BaseDamageColor);
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
		case EType::kSoil:
		{
			for (const auto& Iter : Ref.ElementSet)
			{
				bool bIsBreak = false;
				switch (Iter.Get<0>())
				{
				case EWuXingType::kSoil:
				{
					Lambda(Iter.Get<2>(), BaseDamageColor);
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
		case EType::kTreatment:
		{
			{
				auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FCharacterRisingTips::Get().WidgetSwitcher));
				if (!UIPtr)
				{
					return;
				}
				UIPtr->SetActiveWidgetIndex(kIcon_Treatment);
			}
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterRisingTips::Get().Text));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetText(FText::FromString(FString::Printf(TEXT("+%d"), Ref.DataModify[ECharacterPropertyType::HP].GetCurrentValue())));
			UIPtr->SetColorAndOpacity(FSlateColor(TreatmentColor));
		}
		break;
		}
	}
}
