
#include "TalentAllocation.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"

const FName UsedNum = TEXT("UsedNum");

void UTalentAllocation::NativeConstruct()
{
	Super::NativeConstruct();

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
}

void UTalentAllocation::NativeDestruct()
{
	Super::NativeDestruct();

	if (OnValueChanged)
	{
		OnValueChanged->UnBindCallback();
	}
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

