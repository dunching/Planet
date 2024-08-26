
#include "State_Talent_YinYang.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "Skill_Talent_YinYang.h"
#include "LogWriter.h"
#include "SceneObjSubSystem.h"

namespace State_Talent_YinYang
{
	const FName ProgressBar = TEXT("ProgressBar");

	const FName Text = TEXT("Text");
}

void UState_Talent_YinYang::NativeConstruct()
{
	Super::NativeConstruct();
	PRINTINVOKEINFO();
	if (TargetCharacterPtr)
	{
		auto CharacterAttributesSPtr = TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

		TalentSPtr = TSharedPtr<FCurrentTalentType>(
			CharacterAttributesSPtr->TalentSPtr, dynamic_cast<FCurrentTalentType*>(CharacterAttributesSPtr->TalentSPtr.Get())
		);

		if (TalentSPtr)
		{
			OnValueChangedHandle = TalentSPtr->CallbackContainerHelper.AddOnValueChanged(
				std::bind(&ThisClass::OnCurrentValueChanged, this, std::placeholders::_2)
			);
			OnValueChanged();
		}
	}
}

void UState_Talent_YinYang::NativeDestruct()
{
	PRINTINVOKEINFO();
	if (OnValueChangedHandle)
	{
		OnValueChangedHandle->UnBindCallback();
	}
	Super::NativeDestruct();
}

void UState_Talent_YinYang::OnCurrentValueChanged(int32 CurrentValue)
{
	OnValueChanged();
}

void UState_Talent_YinYang::OnValueChanged()
{
	TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->TalentSPtr;

	const auto CurentType = TalentSPtr->CurentType;
	const auto CurrentValue = TalentSPtr->GetCurrentValue();
	const auto MaxValue = TalentSPtr->GetMaxValue();

	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(State_Talent_YinYang::Text));
		if (UIPtr)
		{
			UIPtr->SetText(
				FText::FromString(*FString::Printf(TEXT("%d/%d"),
					CurrentValue, MaxValue))
			);
		}
	}
	const auto Percent = static_cast<float>(CurrentValue) / MaxValue;
	{
		auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(State_Talent_YinYang::ProgressBar));
		if (UIPtr)
		{
			UIPtr->SetPercent(Percent);
		}
	}

	SetValue(CurentType, CurrentValue, MaxValue);
}
