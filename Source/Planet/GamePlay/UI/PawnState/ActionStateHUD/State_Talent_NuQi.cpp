
#include "State_Talent_NuQi.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "Talent_NuQi.h"

namespace State_Talent_NuQi
{
	const FName ProgressBar = TEXT("ProgressBar");

	const FName Text = TEXT("Text");
}

void UState_Talent_NuQi::NativeConstruct()
{
	Super::NativeConstruct();

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (CharacterPtr)
	{
		auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		auto NuQiPtr = dynamic_cast<FTalent_NuQi*>(CharacterAttributes.TalentPtr);
		if (NuQiPtr)
		{
			OnValueChanged = NuQiPtr->CallbackContainerHelper.AddOnValueChanged(
				std::bind(&ThisClass::OnNuQiCurrentValueChanged, this, std::placeholders::_2)
			);

			NuQiMaxValue = NuQiPtr->GetMaxValue();
		}
	}
}

void UState_Talent_NuQi::NativeDestruct()
{
	if (OnValueChanged)
	{
		OnValueChanged->UnBindCallback();
	}
	Super::NativeDestruct();
}

void UState_Talent_NuQi::OnNuQiCurrentValueChanged(int32 CurrentValue)
{
	NuQiCurrentValue = CurrentValue;
	OnNuQiValueChanged();
}

void UState_Talent_NuQi::OnNuQiValueChanged()
{
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(State_Talent_NuQi::Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(*FString::Printf(TEXT("%d/%d"), NuQiCurrentValue, NuQiMaxValue)));
		}
	}
	const auto Percent = static_cast<float>(NuQiCurrentValue) / NuQiMaxValue;
	{
		auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(State_Talent_NuQi::ProgressBar));
		if (UIPtr)
		{
			UIPtr->SetPercent(Percent);
		}
	}
	SetNuQiPercent(Percent);
}
