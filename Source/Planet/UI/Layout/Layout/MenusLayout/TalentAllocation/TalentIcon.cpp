#include "TalentIcon.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

#include "CharacterBase.h"
#include "DataTableCollection.h"
#include "ItemProxy_Character.h"
#include "TalentAllocationComponent.h"
#include "Components/Border.h"
#include "Kismet/KismetStringLibrary.h"

struct FTalentIcon : public TStructVariable<FTalentIcon>
{
	const FName Level = TEXT("Level");
};

void UTalentIcon::SetIsEnabled(
	bool bInIsEnabled
	)
{
	if (DisEnable)
	{
		DisEnable->SetVisibility(bInIsEnabled ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}

	Super::SetIsEnabled(bInIsEnabled);
}

void UTalentIcon::Reset()
{
	UpdateNum();
}

void UTalentIcon::NativeConstruct()
{
	Super::NativeConstruct();

	const auto TargetTalent = UDataTableCollection::GetInstance()->GetTableRow_TalenSocket(TalentSocket);
	if (TargetTalent && DescriptionText)
	{
		const auto NewStr = DescriptionStr.Replace(
		                                           TEXT("{Value}"),
		                                           *UKismetStringLibrary::Conv_IntToString(TargetTalent->Value)
		                                          );
		DescriptionText->SetText(FText::FromString(NewStr));
	}
}

FReply UTalentIcon::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (OnValueChanged.Execute(this, true))
		{
			UpdateNum();
			return FReply::Handled();
		}
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (OnValueChanged.Execute(this, false))
		{
			UpdateNum();
			return FReply::Handled();
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UTalentIcon::UpdateNum()
{
	if (CurrentProxyPtr)
	{
		const auto& CharacterTalentRef = CurrentProxyPtr->GetCharacterTalent();
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FTalentIcon::Get().Level));
		if (UIPtr)
		{
			int32 Num = 0;
			if (CharacterTalentRef.AllocationMap.Contains(TalentSocket))
			{
				Num = CharacterTalentRef.AllocationMap[TalentSocket];
			}
			else
			{
			}

			UIPtr->SetText(
			               FText::FromString(
			                                 *FString::Printf(
			                                                  TEXT("%d/%d"),
			                                                  Num,
			                                                  MaxNum
			                                                 )
			                                )
			              );
		}
	}
}
