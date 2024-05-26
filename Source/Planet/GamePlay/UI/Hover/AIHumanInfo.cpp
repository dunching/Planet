
#include "AIHumanInfo.h"

#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "HumanCharacter.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "CharacterBase.h"

void UAIHumanInfo::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));

	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("SizeBox")));
	if (SizeBoxPtr)
	{
		SizeBox.X = SizeBoxPtr->GetWidthOverride();
		SizeBox.Y = SizeBoxPtr->GetHeightOverride();
	}

	if (CharacterPtr)
	{
		auto& Ref = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().HP;
		{
			OnHPMaxValueChanged(Ref.GetMaxValue());
			MaxHPValueChanged = Ref.GetMaxProperty().CallbackContainerHelper.AddOnValueChanged(
				std::bind(&ThisClass::OnHPMaxValueChanged, this, std::placeholders::_2)
			);
		}
		{
			OnHPCurrentValueChanged(Ref.GetCurrentValue());
			CurrentHPValueChanged = Ref.GetCurrentProperty().CallbackContainerHelper.AddOnValueChanged(
				std::bind(&ThisClass::OnHPCurrentValueChanged, this, std::placeholders::_2)
			);
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
		auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
		if (WidgetPtr)
		{
			WidgetPtr->SetPercent(static_cast<float>(CurrentHP) / MaxHP);
		}
	}
	{
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
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

	const auto Scale = UWidgetLayoutLibrary::GetViewportScale(this);

	const auto TempWidgetSize = SizeBox * Scale;

	ScreenPosition.X -= (TempWidgetSize.X / 2);

	DesiredPt = ScreenPosition;

	// 	const auto OffsetVec = DesiredPt - PreviousPt;
	// 
	// 	if (OffsetVec.Length() > 10.f)
	// 	{
	// 		DesiredPt = PreviousPt + (OffsetVec.GetSafeNormal() * 10.f);
	// 	}
	// 
	// 	PreviousPt = DesiredPt;
	SetPositionInViewport(DesiredPt);

	return true;
}
