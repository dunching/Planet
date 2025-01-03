#include "FocusTitle.h"

#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include <Components/CanvasPanel.h>
#include <Components/CanvasPanelSlot.h>
#include <GameplayTagsManager.h>

#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Border.h"

#include "HumanCharacter.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"
#include "EffectsList.h"

struct FFocusTitle : public TStructVariable<FFocusTitle>
{
	const FName HP_ProgressBar = TEXT("HP_ProgressBar");

	const FName PP_ProgressBar = TEXT("PP_ProgressBar");

	const FName Shield_ProgressBar = TEXT("Shield_ProgressBar");

	const FName Border = TEXT("Border");

	const FName Text = TEXT("Text");

	const FName Title = TEXT("Title");

	const FName CanvasPanel = TEXT("CanvasPanel");

	const FName EffectsList = TEXT("EffectsList");
};

void UFocusTitle::NativeConstruct()
{
	Super::NativeConstruct();
}

void UFocusTitle::NativeDestruct()
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
			auto GASCompPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			GASCompPtr->RegisterGenericGameplayTagEvent().Remove(OnGameplayEffectTagCountChangedHandle);
		}
	}

	Super::NativeDestruct();
}

void UFocusTitle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UFocusTitle::SetTargetCharacter(ACharacterBase* TargetCharacterPtr)
{
	CharacterPtr = TargetCharacterPtr;
	if (CharacterPtr)
	{
		float Radius = 0.f;
		CharacterPtr->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);

		{
			auto GASCompPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			OnGameplayEffectTagCountChangedHandle = GASCompPtr->RegisterGenericGameplayTagEvent().AddUObject(
				this, &ThisClass::OnGameplayEffectTagCountChanged);
		}

		auto CharacterAttributesPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		auto AbilitySystemComponentPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		{
			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributesPtr->GetHPAttribute()
			).AddUObject(this, &ThisClass::OnHPChanged);
			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributesPtr->GetMax_HPAttribute()
			).AddUObject(this, &ThisClass::OnHPChanged);
			SetHPChanged(CharacterAttributesPtr->GetHP(), CharacterAttributesPtr->GetMax_HP());
		}
		{
			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributesPtr->GetPPAttribute()
			).AddUObject(this, &ThisClass::OnPPChanged);
			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributesPtr->GetMax_PPAttribute()
			).AddUObject(this, &ThisClass::OnPPChanged);
			SetPP(CharacterAttributesPtr->GetPP(), CharacterAttributesPtr->GetMax_PP());
		}
		{
			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributesPtr->GetShieldAttribute()
			).AddUObject(this, &ThisClass::OnShieldChanged);
			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributesPtr->GetMax_HPAttribute()
			).AddUObject(this, &ThisClass::OnShieldChanged);
			SetShild(CharacterAttributesPtr->GetShield(), CharacterAttributesPtr->GetMax_HP());
		}

		auto EffectPtr = Cast<UEffectsList>(GetWidgetFromName(FFocusTitle::Get().EffectsList));
		if (EffectPtr)
		{
			EffectPtr->BindCharacterState(TargetCharacterPtr);
		}
	}
	SwitchCantBeSelect(false);

	ApplyCharaterNameToTitle();
}

void UFocusTitle::SwitchCantBeSelect(bool bIsCanBeSelect)
{
	auto WidgetPtr = Cast<UBorder>(GetWidgetFromName(FFocusTitle::Get().Border));
	if (WidgetPtr)
	{
		WidgetPtr->SetContentColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, bIsCanBeSelect ? .3f : 1.f));
	}
}

void UFocusTitle::OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count)
{
	if (Tag.MatchesTagExact(UGameplayTagsLibrary::Debuff))
	{
	}
	else if (Tag.MatchesTag(UGameplayTagsLibrary::Debuff))
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

void UFocusTitle::OnHPChanged(const FOnAttributeChangeData& CurrentValue)
{
	auto CharacterAttributesPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	SetHPChanged(CharacterAttributesPtr->GetHP(), CharacterAttributesPtr->GetMax_HP());
}

void UFocusTitle::SetHPChanged(float Value, float MaxValue)
{
	{
		auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FFocusTitle::Get().HP_ProgressBar));
		if (WidgetPtr)
		{
			WidgetPtr->SetPercent(static_cast<float>(CurrentHP) / MaxHP);
		}
	}
	{
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(FFocusTitle::Get().Text));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentHP, MaxHP)));
		}
	}
}

void UFocusTitle::OnPPChanged(const FOnAttributeChangeData& CurrentValue)
{
	auto CharacterAttributesPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	SetHPChanged(CharacterAttributesPtr->GetPP(), CharacterAttributesPtr->GetMax_PP());
}

void UFocusTitle::SetPP(float Value, float MaxValue)
{
	auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FFocusTitle::Get().PP_ProgressBar));
	if (WidgetPtr)
	{
		WidgetPtr->SetPercent(static_cast<float>(Value) / MaxValue);
	}
}

void UFocusTitle::OnShieldChanged(const FOnAttributeChangeData& CurrentValue)
{
	auto CharacterAttributesPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
			SetShild(CharacterAttributesPtr->GetShield(), CharacterAttributesPtr->GetMax_HP());
}

void UFocusTitle::SetShild(float Value, float MaxValue)
{
	auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FFocusTitle::Get().Shield_ProgressBar));
	if (WidgetPtr)
	{
		WidgetPtr->SetPercent(static_cast<float>(Value) / MaxValue);
	}
}

void UFocusTitle::ApplyCharaterNameToTitle()
{
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FFocusTitle::Get().Title));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(CharacterPtr->GetCharacterProxy()->Name));
		}
	}
}

void UFocusTitle::ApplyStatesToTitle()
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
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FFocusTitle::Get().Title));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(Title));
		}
	}
}

bool UFocusTitle::ResetPosition(float InDeltaTime)
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
