
#include "TalentAllocation.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"
#include "TalentIcon.h"

const FName UsedNum = TEXT("UsedNum");

void UTalentAllocation::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTalentAllocation::NativeDestruct()
{
	Super::NativeDestruct(); 

	if (OnValueChanged)
	{
		OnValueChanged->UnBindCallback();
	}
	for (auto& Iter : OnPointChangedHandleAry)
	{
		if (Iter)
		{
			Iter->UnBindCallback();
		}
	}

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (CharacterPtr)
	{
		auto TalentAllocationComponentPtr = CharacterPtr->GetTalentAllocationComponent();
		if (TalentAllocationComponentPtr)
		{
			TalentAllocationComponentPtr->SyncToHolding();
		}
	}

}

void UTalentAllocation::EnableMenu()
{
	if (WidgetTree)
	{
	}

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (CharacterPtr)
	{
		auto TalentAllocationComponentPtr = CharacterPtr->GetTalentAllocationComponent();
		if (TalentAllocationComponentPtr)
		{
			OnValueChanged = TalentAllocationComponentPtr->CallbackContainerHelper.AddOnValueChanged(
				std::bind(&ThisClass::OnUsedTalentNumChanged, this, std::placeholders::_1, std::placeholders::_2)
			);
		}
	}

	if (WidgetTree)
	{
		WidgetTree->ForEachWidget([this](UWidget* Widget) {
			if (Widget && Widget->IsA<UTalentIcon>())
			{
				auto UIPtr = Cast<UTalentIcon>(Widget);
				if (!UIPtr)
				{
					return;
				}

				OnPointChangedHandleAry.Add(UIPtr->OnValueChanged.AddCallback(
					std::bind(&ThisClass::OnAddPoint, this, std::placeholders::_1, std::placeholders::_2)
				));
			}
			});
	}
}

void UTalentAllocation::DisEnableMenu()
{
}

EMenuType UTalentAllocation::GetMenuType() const
{
	return EMenuType::kAllocationTalent;
}

void UTalentAllocation::OnUsedTalentNumChanged(int32 OldNum, int32 NewNum)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(UsedNum));
	if (UIPtr)
	{
		auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (CharacterPtr)
		{
			auto TalentAllocationComponentPtr = CharacterPtr->GetTalentAllocationComponent();
			if (TalentAllocationComponentPtr)
			{
				UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), NewNum, TalentAllocationComponentPtr->GetTotalTalentPointNum())));
			}
		}
	}
}

void UTalentAllocation::OnAddPoint(UTalentIcon* TalentIconPtr, bool bIsAdd)
{
	if (TalentIconPtr && bIsAdd)
	{
		if (WidgetTree)
		{
			WidgetTree->ForEachWidget([this, TalentIconPtr](UWidget* Widget) {
				if (Widget && Widget->IsA<UTalentIcon>())
				{
					auto UIPtr = Cast<UTalentIcon>(Widget);
					if (!UIPtr)
					{
						return;
					}

					if (UIPtr == TalentIconPtr)
					{
					}
					else
					{
						auto TalentHelper = UIPtr->GetTalentHelper();
						if (TalentHelper.PointType == EPointType::kSkill)
						{
							UIPtr->ResetPoint();
						}
					}
				}
				});
		}
	}
}

