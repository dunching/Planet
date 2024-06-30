
#include "AIHumanInfo.h"

#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include <Components/CanvasPanel.h>
#include <Components/CanvasPanelSlot.h>

#include "HumanCharacter.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "CharacterBase.h"

namespace AIHumanInfo
{
	const FName ProgressBar = TEXT("ProgressBar");

	const FName Text = TEXT("Text");

	const FName Title = TEXT("Title");

	const FName CanvasPanel = TEXT("CanvasPanel");
}

void UAIHumanInfo::NativeConstruct()
{
	Super::NativeConstruct();

	Scale = UWidgetLayoutLibrary::GetViewportScale(this);

	SetAnchorsInViewport(FAnchors(.5f));

	if (CharacterPtr)
	{
		auto& Ref = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().HP;
		{
			OnHPMaxValueChanged(Ref.GetMaxValue());
			MaxHPValueChanged = Ref.AddOnMaxValueChanged(
				std::bind(&ThisClass::OnHPMaxValueChanged, this, std::placeholders::_2)
			);
		}
		{
			OnHPCurrentValueChanged(Ref.GetCurrentValue());
			CurrentHPValueChanged = Ref.AddOnValueChanged(
				std::bind(&ThisClass::OnHPCurrentValueChanged, this, std::placeholders::_2)
			);
		}
	}

	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(AIHumanInfo::Title));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromName(CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Name));
		}
	}

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition));
	ResetPosition(0.f);
}

void UAIHumanInfo::NativeDestruct()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	if (MaxHPValueChanged)
	{
		MaxHPValueChanged->UnBindCallback();
	}
	if (CurrentHPValueChanged)
	{
		CurrentHPValueChanged->UnBindCallback();
	}

	Super::NativeDestruct();
}

void UAIHumanInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UAIHumanInfo::OnHPCurrentValueChanged(int32 NewVal)
{
	CurrentHP = NewVal;
	OnHPChanged();
}

void UAIHumanInfo::OnHPMaxValueChanged(int32 NewVal)
{
	MaxHP = NewVal;
	OnHPChanged();
}

void UAIHumanInfo::OnHPChanged()
{
	{
		auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(AIHumanInfo::ProgressBar));
		if (WidgetPtr)
		{
			WidgetPtr->SetPercent(static_cast<float>(CurrentHP) / MaxHP);
		}
	}
	{
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(AIHumanInfo::Text));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentHP, MaxHP)));
		}
	}
}

bool UAIHumanInfo::ResetPosition(float InDeltaTime)
{
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(
		UGameplayStatics::GetPlayerController(this, 0),
		CharacterPtr->GetActorLocation() - (CharacterPtr->GetGravityDirection() * Offset),
		ScreenPosition
	);

	if (Size.IsNearlyZero())
	{
		auto UIPtr = Cast<UCanvasPanel>(GetWidgetFromName(AIHumanInfo::CanvasPanel));
		if (UIPtr)
		{
			TSharedPtr<SPanel> PanelSPtr = UIPtr->GetCanvasWidget();
			Size = PanelSPtr->ComputeDesiredSize(0.f);
		}
	}

	const auto TempWidgetSize = Size * Scale;

	ScreenPosition.X -= (TempWidgetSize.X / 2);

	DesiredPt = ScreenPosition;

	SetPositionInViewport(DesiredPt);

	return true;
}
