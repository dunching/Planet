
#include "TalentIcon.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"

namespace TalentIcon
{
	const FName Level = TEXT("Level");
}

void UTalentIcon::ResetPoint()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (CharacterPtr)
	{
		auto TalentAllocationComponentPtr = CharacterPtr->GetTalentAllocationComponent();
		if (TalentAllocationComponentPtr)
		{
			FTalentHelper TalentHelper = GetTalentHelper();

			TalentAllocationComponentPtr->Clear(TalentHelper);

			ResetUI(TalentHelper);
		}
	}
}

void UTalentIcon::NativeConstruct()
{
	Super::NativeConstruct();

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (CharacterPtr)
	{
		auto TalentAllocationComponentPtr = CharacterPtr->GetTalentAllocationComponent();
		if (TalentAllocationComponentPtr)
		{
			FTalentHelper TalentHelper = GetTalentHelper();

			auto ResultPtr = TalentAllocationComponentPtr->GetCheck(TalentHelper);
			if (ResultPtr)
			{
				ResetUI(*ResultPtr);
			}
			else
			{
				FTalentHelper TempTalentHelper;
				TempTalentHelper.PointType = EPointType::kNone;

				ResetUI(TempTalentHelper);
			}
		}
	}
}

FReply UTalentIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (CharacterPtr)
	{
		auto TalentAllocationComponentPtr = CharacterPtr->GetTalentAllocationComponent();
		if (TalentAllocationComponentPtr)
		{
			if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
			{
				FTalentHelper TalentHelper = GetTalentHelper();

				auto Result = TalentAllocationComponentPtr->AddCheck(TalentHelper);

				ResetUI(Result);

				OnValueChanged.ExcuteCallback(this, true);
			}
			else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
			{
				FTalentHelper TalentHelper = GetTalentHelper();

				auto Result = TalentAllocationComponentPtr->SubCheck(TalentHelper);

				ResetUI(Result);
			}
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UTalentIcon::ResetUI(const FTalentHelper& TalentHelper)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(TalentIcon::Level));
	if (UIPtr)
	{
		switch (TalentHelper.PointType)
		{
		case EPointType::kSkill:
		case EPointType::kProperty:
		{
			UIPtr->SetText(FText::FromString(*FString::Printf(TEXT("%d/%d"), TalentHelper.Level, TalentHelper.TotalLevel)));
		}
		break;
		default:
		{
			UIPtr->SetText(FText::FromString(TEXT("")));
		}
		break;
		}
	}
}

FTalentHelper UTalentIcon::GetTalentHelper() const
{
	FTalentHelper TalentHelper;

	TalentHelper.PointType = PointType;
	TalentHelper.IconSocket = IconSocket;
	switch (PointType)
	{
	case EPointType::kSkill:
	{
		TalentHelper.Type = PointSkillType;
	}
	break;
	case EPointType::kProperty:
	{
		TalentHelper.Type = PointPropertyType;
	}
	break;
	default:
		break;
	}

	return TalentHelper;
}
