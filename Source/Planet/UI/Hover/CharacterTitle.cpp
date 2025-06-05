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
#include "GenerateTypes.h"
#include "CharacterBase.h"
#include "ConversationBorder.h"
#include "GameplayTagsLibrary.h"

struct FCharacterTitle : public TStructVariable<FCharacterTitle>
{
	const FName HP_ProgressBar = TEXT("HP_ProgressBar");

	const FName Stamina_ProgressBar = TEXT("Stamina_ProgressBar");

	const FName Mana_ProgressBar = TEXT("Mana_ProgressBar");

	const FName ProgressBar_Shield = TEXT("ProgressBar_Shield");

	const FName Border = TEXT("Border");

	const FName Text = TEXT("Text");

	const FName Title = TEXT("Title");

	const FName CanvasPanel = TEXT("CanvasPanel");

	const FName LevelText = TEXT("LevelText");
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
		MaxHPValueChanged.Reset();
	}

	if (CurrentHPValueChanged)
	{
		CurrentHPValueChanged.Reset();
	}

	if (LevelChangedDelegateHandle)
	{
		LevelChangedDelegateHandle.Reset();
	}

	for (auto Iter : ValueChangedAry)
	{
		if (Iter)
		{
			Iter.Reset();
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

void UCharacterTitle::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
	)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCharacterTitle::SwitchCantBeSelect(
	bool bIsCantBeSelect
	)
{
	auto WidgetPtr = Cast<UBorder>(GetWidgetFromName(FCharacterTitle::Get().Border));
	if (WidgetPtr)
	{
		WidgetPtr->SetContentColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, bIsCantBeSelect ? .3f : 1.f));
	}
}

void UCharacterTitle::OnGameplayEffectTagCountChanged(
	const FGameplayTag Tag,
	int32 Count
	)
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

void UCharacterTitle::OnHPChanged(
	const FOnAttributeChangeData&OnAttributeChangeData
	)
{
	const auto Value = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetHP();
	const auto MaxValue = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetMax_HP();
	SetHPChanged(Value, MaxValue);
}

void UCharacterTitle::SetHPChanged(
	float Value,
	float MaxValue
	)
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

void UCharacterTitle::OnStaminaChanged(
	const FOnAttributeChangeData&OnAttributeChangeData
	)
{
	const auto Value = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetStamina();
	const auto MaxValue = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetMax_Stamina();
	SetStaminaChanged(Value, MaxValue);
}

void UCharacterTitle::SetStaminaChanged(
	float Value,
	float MaxValue
	)
{
	auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().Stamina_ProgressBar));
	if (WidgetPtr)
	{
		WidgetPtr->SetPercent(static_cast<float>(Value) / MaxValue);
	}
}

void UCharacterTitle::OnManaChanged(
	const FOnAttributeChangeData& OnAttributeChangeData
	)
{
	const auto Value = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetMana();
	const auto MaxValue = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetMax_Mana();
	SetManaChanged(Value, MaxValue);
}

void UCharacterTitle::SetManaChanged(
	float Value,
	float MaxValue
	)
{
	auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().Mana_ProgressBar));
	if (WidgetPtr)
	{
		WidgetPtr->SetPercent(static_cast<float>(Value) / MaxValue);
	}
}

void UCharacterTitle::OnShieldChanged(
	const FOnAttributeChangeData&OnAttributeChangeData
	)
{
	const auto Value = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetShield();
	const auto MaxValue = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetMax_HP();
	SetShieldChanged(Value, MaxValue);
}

void UCharacterTitle::SetShieldChanged(
	float Value,
	float MaxValue
	)
{
	if (MaxValue > 0)
	{
		auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().ProgressBar_Shield));
		if (WidgetPtr)
		{
			WidgetPtr->SetPercent(FMath::Clamp(static_cast<float>(Value) / MaxValue, 0, 1));
		}
	}
	else
	{
		auto WidgetPtr = Cast<UProgressBar>(GetWidgetFromName(FCharacterTitle::Get().ProgressBar_Shield));
		if (WidgetPtr)
		{
			WidgetPtr->SetPercent(0);
		}
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

void UCharacterTitle::ApplyLevelToTitle()
{
	if (!CharacterPtr)
	{
		return;
	}

	auto CharacterProxySPtr = CharacterPtr->GetCharacterProxy();
	if (!CharacterProxySPtr)
	{
		return;
	}

	LevelChangedDelegateHandle = CharacterProxySPtr->LevelChangedDelegate.AddOnValueChanged(
		 std::bind(&ThisClass::OnLevelChanged, this, std::placeholders::_2)
		);
	OnLevelChanged(CharacterProxySPtr->GetLevel());
}

bool UCharacterTitle::ResetPosition(
	float InDeltaTime
	)
{
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(
	                                       UGameplayStatics::GetPlayerController(this, 0),
	                                       CharacterPtr->GetActorLocation() - (
		                                       CharacterPtr->GetGravityDirection() * (Offset + HalfHeight)),
	                                       ScreenPosition
	                                      );

	SetPositionInViewport(ScreenPosition);

	return true;
}

void UCharacterTitle::OnLevelChanged(
	int32 Level
	)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterTitle::Get().LevelText));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%d"), Level)));
}

void UCharacterTitle::SetData(
	ACharacterBase* InCharacterPtr
	)
{
	CharacterPtr = InCharacterPtr;
	if (CharacterPtr)
	{
		float Radius = 0.f;
		CharacterPtr->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);

		{
			auto GASCompPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			OnGameplayEffectTagCountChangedHandle = GASCompPtr->RegisterGenericGameplayTagEvent().AddUObject(
				 this,
				 &ThisClass::OnGameplayEffectTagCountChanged
				);
		}

		auto CharacterAttributeSetPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		auto AbilitySystemComponentPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
				 CharacterAttributeSetPtr->GetHPAttribute()
				).AddUObject(this, &ThisClass::OnHPChanged);

			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
			                                                                   CharacterAttributeSetPtr->
			                                                                   GetMax_HPAttribute()
			                                                                  ).AddUObject(
				 this,
				 &ThisClass::OnHPChanged
				);

			SetHPChanged(CharacterAttributeSetPtr->GetHP(), CharacterAttributeSetPtr->GetMax_HP());
		}
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
				 CharacterAttributeSetPtr->GetStaminaAttribute()
				).AddUObject(this, &ThisClass::OnStaminaChanged);

			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
			                                                                   CharacterAttributeSetPtr->
			                                                                   GetMax_StaminaAttribute()
			                                                                  ).AddUObject(
				 this,
				 &ThisClass::OnStaminaChanged
				);

			SetStaminaChanged(CharacterAttributeSetPtr->GetStamina(), CharacterAttributeSetPtr->GetMax_Stamina());
		}
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
				 CharacterAttributeSetPtr->GetManaAttribute()
				).AddUObject(this, &ThisClass::OnManaChanged);

			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
			                                                                   CharacterAttributeSetPtr->
			                                                                   GetMax_ManaAttribute()
			                                                                  ).AddUObject(
				 this,
				 &ThisClass::OnManaChanged
				);

			SetManaChanged(CharacterAttributeSetPtr->GetMana(), CharacterAttributeSetPtr->GetMax_Mana());
		}
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
				 CharacterAttributeSetPtr->GetShieldAttribute()
				).AddUObject(this, &ThisClass::OnShieldChanged);

			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
			                                                                   CharacterAttributeSetPtr->
			                                                                   GetMax_HPAttribute()
			                                                                  ).AddUObject(
				 this,
				 &ThisClass::OnShieldChanged
				);

			SetShieldChanged(CharacterAttributeSetPtr->GetShield(), CharacterAttributeSetPtr->GetMax_HP());
		}
		SwitchCantBeSelect(false);

		ApplyCharaterNameToTitle();

		ApplyLevelToTitle();
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

void UCharacterTitleBox::SetData(
	ACharacterBase* CharacterPtr_
	)
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
		                                GetWidgetFromName(FCharacterTitleBox::Get().VerticalBox)
		                               );
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
			                                GetWidgetFromName(FCharacterTitleBox::Get().VerticalBox)
			                               );
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
