
#include "CharacterTitle.h"

#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include <Components/CanvasPanel.h>
#include <Components/CanvasPanelSlot.h>
#include <GameplayTagsManager.h>
#include "Components/CapsuleComponent.h"
#include "Components/Border.h"

#include "HumanCharacter.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "CharacterBase.h"
#include "GameplayTagsSubSystem.h"

struct FCharacterTitle : public TStructVariable<FCharacterTitle>
{
	const FName HP_ProgressBar = TEXT("HP_ProgressBar");

	const FName PP_ProgressBar = TEXT("PP_ProgressBar");

	const FName Shield_ProgressBar = TEXT("Shield_ProgressBar");

	const FName Border = TEXT("Border");

	const FName Text = TEXT("Text");

	const FName Title = TEXT("Title");

	const FName CanvasPanel = TEXT("CanvasPanel");
};

void UCharacterTitle::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));
	SetAlignmentInViewport(FVector2D(.5f, 1.f));

	if (CharacterPtr)
	{
		float Radius = 0.f;
		CharacterPtr->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);

		{
			auto GASCompPtr = CharacterPtr->GetAbilitySystemComponent(); 
			OnGameplayEffectTagCountChangedHandle = GASCompPtr->RegisterGenericGameplayTagEvent().AddUObject(this, &ThisClass::OnGameplayEffectTagCountChanged);
		}
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
			auto& Ref = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().PP;
			ValueChangedAry.Add(Ref.AddOnMaxValueChanged(
				std::bind(&ThisClass::OnPPChanged, this)
			));
			ValueChangedAry.Add(Ref.AddOnValueChanged(
				std::bind(&ThisClass::OnPPChanged, this)
			));
			OnPPChanged();
		}
		{
			auto& Ref = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Shield;
			ValueChangedAry.Add(Ref.AddOnMaxValueChanged(
				std::bind(&ThisClass::OnShieldChanged, this)
			));
			ValueChangedAry.Add(Ref.AddOnValueChanged(
				std::bind(&ThisClass::OnShieldChanged, this)
			));
			OnShieldChanged();
		}
		SwitchCantBeSelect(false);

		ApplyCharaterNameToTitle();

		TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition));
		ResetPosition(0.f);
	}
}

void UCharacterTitle::NativeDestruct()
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

	for (auto Iter : ValueChangedAry)
	{
		if (Iter)
		{
			Iter->UnBindCallback();
		}
	}

	if (CharacterPtr)
	{
		{
			auto GASCompPtr = CharacterPtr->GetAbilitySystemComponent();
			GASCompPtr->RegisterGenericGameplayTagEvent().Remove(OnGameplayEffectTagCountChangedHandle);
		}
	}

	Super::NativeDestruct();
}

void UCharacterTitle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCharacterTitle::SwitchCantBeSelect(bool bIsCanBeSelect)
{
	auto WidgetPtr = Cast<UBorder>(GetWidgetFromName(FCharacterTitle::Get().Border));
	if (WidgetPtr)
	{
		WidgetPtr->SetContentColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, bIsCanBeSelect ? .3f: 1.f));
	}
}

void UCharacterTitle::OnHPCurrentValueChanged(int32 NewVal)
{
	CurrentHP = NewVal;
	OnHPChanged();
}

void UCharacterTitle::OnHPMaxValueChanged(int32 NewVal)
{
	MaxHP = NewVal;
	OnHPChanged();
}

void UCharacterTitle::OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count)
{
	if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->Debuff))
	{
	}
	else if (Tag.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Debuff))
	{
		if (Count > 0)
		{
			TagSet.Add(Tag);
		}
		else
		{
			TagSet.Remove(Tag);
		}

		if (TagSet.IsEmpty())
		{
			ApplyCharaterNameToTitle();
		}
		else
		{
			ApplyStatesToTitle();
		}
	}
}

void UCharacterTitle::OnHPChanged()
{
	{
		auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().HP_ProgressBar));
		if (WidgetPtr)
		{
			WidgetPtr->SetPercent(static_cast<float>(CurrentHP) / MaxHP);
		}
	}
	{
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterTitle::Get().Text));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentHP, MaxHP)));
		}
	}
}

void UCharacterTitle::OnPPChanged()
{
	auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().PP_ProgressBar));
	if (WidgetPtr)
	{
		auto& Ref = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().PP;
		WidgetPtr->SetPercent(static_cast<float>(Ref.GetCurrentValue()) / Ref.GetMaxValue());
	}
}

void UCharacterTitle::OnShieldChanged()
{
	auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().Shield_ProgressBar));
	if (WidgetPtr)
	{
		auto& Ref = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Shield;
		WidgetPtr->SetPercent(static_cast<float>(Ref.GetCurrentValue()) / Ref.GetMaxValue());
	}
}

void UCharacterTitle::ApplyCharaterNameToTitle()
{
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterTitle::Get().Title));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Name));
		}
	}
}

void UCharacterTitle::ApplyStatesToTitle()
{
	FString Title;
	for (const auto Iter : TagSet)
	{
		TSharedPtr<FGameplayTagNode> TagNode = UGameplayTagsManager::Get().FindTagNode(Iter);
		if (TagNode.IsValid())
		{
			Title.Append(TagNode->GetSimpleTagName().ToString());
			Title.Append(TEXT(","));
		}
	}
	if (!Title.IsEmpty())
	{
		Title.RemoveAt(Title.Len() - 1);
	}

	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterTitle::Get().Title));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(Title));
		}
	}
}

bool UCharacterTitle::ResetPosition(float InDeltaTime)
{
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(
		UGameplayStatics::GetPlayerController(this, 0),
		CharacterPtr->GetActorLocation() - (CharacterPtr->GetGravityDirection() * (Offset + HalfHeight)),
		ScreenPosition
	);

	SetPositionInViewport(ScreenPosition);

	return true;
}
