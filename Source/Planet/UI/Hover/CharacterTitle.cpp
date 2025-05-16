
#include "CharacterTitle.h"

#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include <Components/CanvasPanel.h>
#include <Components/CanvasPanelSlot.h>
#include <GameplayTagsManager.h>
#include "Components/VerticalBox.h"

#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Border.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"

#include "HumanCharacter.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "CharacterBase.h"
#include "ConversationBorder.h"
#include "GameplayTagsLibrary.h"

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

	// SetAnchorsInViewport(FAnchors(.5f));
	// SetAlignmentInViewport(FVector2D(.5f, 1.f));
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
			auto GASCompPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			GASCompPtr->RegisterGenericGameplayTagEvent().Remove(OnGameplayEffectTagCountChangedHandle);
		}
	}

	Super::NativeDestruct();
}

void UCharacterTitle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCharacterTitle::SwitchCantBeSelect(bool bIsCantBeSelect)
{
	auto WidgetPtr = Cast<UBorder>(GetWidgetFromName(FCharacterTitle::Get().Border));
	if (WidgetPtr)
	{
		WidgetPtr->SetContentColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, bIsCantBeSelect ? .3f: 1.f));
	}
}

void UCharacterTitle::OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count)
{
	auto Lambda = [&]
	{
		const auto Value = Count > 0;
		return Value;
	};

	if (Tag.MatchesTagExact(UGameplayTagsLibrary::Debuff))
	{
	}
	else if (Tag.MatchesTag(UGameplayTagsLibrary::Debuff))
	{
		if (Lambda())
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
	else if (Tag.MatchesTag(UGameplayTagsLibrary::State_Buff_CantBeSlected))
	{
		SwitchCantBeSelect(Lambda());
	}
}

void UCharacterTitle::OnHPChanged(const FOnAttributeChangeData& )
{
	const auto Value = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetHP();
	const auto MaxValue = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetMax_HP();
	SetHPChanged(Value, MaxValue);
}

void UCharacterTitle::SetHPChanged(float Value, float MaxValue)
{
	{
		auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().HP_ProgressBar));
		if (WidgetPtr)
		{
			WidgetPtr->SetPercent(static_cast<float>(Value) / MaxValue);
		}
	}
	{
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterTitle::Get().Text));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(FString::Printf(TEXT("%.0lf/%.0lf"), Value, MaxValue)));
		}
	}
}

void UCharacterTitle::OnPPChanged(const FOnAttributeChangeData&)
{
	const auto Value = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetStamina();
	const auto MaxValue = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetMax_Stamina();
	SetPPChanged(Value, MaxValue);
}

void UCharacterTitle::SetPPChanged(float Value, float MaxValue)
{
	auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().PP_ProgressBar));
	if (WidgetPtr)
	{
		WidgetPtr->SetPercent(static_cast<float>(Value) / MaxValue);
	}
}

void UCharacterTitle::OnShieldChanged(const FOnAttributeChangeData&)
{
	const auto Value = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetShield();
	const auto MaxValue = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetMax_HP();
	SetPPChanged(Value, MaxValue);
}

void UCharacterTitle::SetShieldChanged(float Value, float MaxValue)
{
	auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().Shield_ProgressBar));
	if (WidgetPtr)
	{
		WidgetPtr->SetPercent(static_cast<float>(Value) / MaxValue);
	}
}

void UCharacterTitle::ApplyCharaterNameToTitle()
{
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterTitle::Get().Title));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(CharacterPtr->GetCharacterProxy()->GetDisplayTitle()));
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

void UCharacterTitle::SetData(ACharacterBase* InCharacterPtr)
{
	CharacterPtr = InCharacterPtr;
	if (CharacterPtr)
	{
		float Radius = 0.f;
		CharacterPtr->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);

		{
			auto GASCompPtr = CharacterPtr->GetCharacterAbilitySystemComponent(); 
			OnGameplayEffectTagCountChangedHandle = GASCompPtr->RegisterGenericGameplayTagEvent().AddUObject(this, &ThisClass::OnGameplayEffectTagCountChanged);
		}
		
		auto CharacterAttributeSetPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		auto AbilitySystemComponentPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributeSetPtr->GetHPAttribute()
				).AddUObject(this, &ThisClass::OnHPChanged);

			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributeSetPtr->GetMax_HPAttribute()
				).AddUObject(this, &ThisClass::OnHPChanged);

			SetHPChanged(CharacterAttributeSetPtr->GetHP(), CharacterAttributeSetPtr->GetMax_HP());
		}
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributeSetPtr->GetStaminaAttribute()
				).AddUObject(this, &ThisClass::OnPPChanged);

			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributeSetPtr->GetMax_StaminaAttribute()
				).AddUObject(this, &ThisClass::OnPPChanged);

			SetPPChanged(CharacterAttributeSetPtr->GetStamina(), CharacterAttributeSetPtr->GetMax_Stamina());
		}
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributeSetPtr->GetShieldAttribute()
				).AddUObject(this, &ThisClass::OnShieldChanged);

			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
				CharacterAttributeSetPtr->GetMax_HPAttribute()
				).AddUObject(this, &ThisClass::OnShieldChanged);

			SetShieldChanged(CharacterAttributeSetPtr->GetShield(), CharacterAttributeSetPtr->GetMax_HP());
		}
		SwitchCantBeSelect(false);

		ApplyCharaterNameToTitle();

		// TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition));
		// ResetPosition(0.f);
	}
}

struct FCharacterTitleBox : public TStructVariable<FCharacterTitleBox>
{
	const FName VerticalBox = TEXT("VerticalBox");

	const FName CharacterTitle = TEXT("CharacterTitle");
};

void UCharacterTitleBox::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UCharacterTitleBox::NativeConstruct()
{
	Super::NativeConstruct();

	// SetAnchorsInViewport(FAnchors(.5f));
	// SetAlignmentInViewport(FVector2D(.5f, 1.f));
}

void UCharacterTitleBox::SetCampType(
	ECharacterCampType CharacterCampType
)
{
	auto UIPtr = Cast<UCharacterTitle>(
		GetWidgetFromName(FCharacterTitleBox::Get().CharacterTitle)
	);
	if (UIPtr)
	{
		UIPtr->SetCampType(CharacterCampType);
	}
}

void UCharacterTitleBox::SetData(ACharacterBase* CharacterPtr_)
{
	CharacterPtr = CharacterPtr_;
	{
		auto UIPtr = Cast<UCharacterTitle>(GetWidgetFromName(FCharacterTitleBox::Get().CharacterTitle));
		if (UIPtr)
		{
			UIPtr->SetData(CharacterPtr);
		}
	}
	{
		auto UIPtr = Cast<UVerticalBox>(
			GetWidgetFromName(FCharacterTitleBox::Get().VerticalBox));
		if (UIPtr)
		{
			UIPtr->ClearChildren();
		}
	}
}

void UCharacterTitleBox::DisplaySentence(
	const FTaskNode_Conversation_SentenceInfo& Sentence
)
{
	if (ConversationBorderPtr)
	{
		ConversationBorderPtr->SetSentence(Sentence);
	}
	else
	{
		ConversationBorderPtr = CreateWidget<UConversationBorder>(GetWorld(), ConversationBorderClass);
		if (ConversationBorderPtr)
		{
			ConversationBorderPtr->CharacterPtr = CharacterPtr;
			ConversationBorderPtr->SetSentence(Sentence);
			auto UIPtr = Cast<UVerticalBox>(
				GetWidgetFromName(FCharacterTitleBox::Get().VerticalBox));
			if (UIPtr)
			{
				UIPtr->AddChild(ConversationBorderPtr);
			}
		}
	}
}

void UCharacterTitleBox::CloseConversationborder()
{
	if (ConversationBorderPtr)
	{
		ConversationBorderPtr->RemoveFromParent();
		ConversationBorderPtr = nullptr;
	}
}
