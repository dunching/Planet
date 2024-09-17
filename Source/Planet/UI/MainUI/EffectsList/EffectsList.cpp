
#include "EffectsList.h"

#include "MyWrapBox.h"
#include "EffectItem.h"
#include "CS_Base.h"
#include "CharacterBase.h"
#include "BaseFeatureGAComponent.h"
#include "StateProcessorComponent.h"

void UEffectsList::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UEffectsList::NativeDestruct()
{
	if (CallbackHandle)
	{
		CallbackHandle->UnBindCallback();
	}

	Super::NativeDestruct();
}

UEffectItem* UEffectsList::AddEffectItem()
{
	auto UIPtr = Cast<UMyWrapBox>(GetWidgetFromName(TEXT("WrapBox")));
	if (UIPtr)
	{
		auto ChildPtr = CreateWidget<UEffectItem>(this, EffectItemClass);
		auto SlotPtr = UIPtr->AddChildToWrapBox(ChildPtr);

		return ChildPtr;
	}

	return nullptr;
}

void UEffectsList::BindCharacterState(ACharacterBase* TargetCharacterPtr)
{
	CallbackHandle = TargetCharacterPtr->GetStateProcessorComponent()->CharacterStateChangedContainer.AddCallback(
		std::bind(&ThisClass::OnCharacterStateChanged, this, std::placeholders::_1, std::placeholders::_2)
	);

	CharacterStateMapHandle = TargetCharacterPtr->GetStateProcessorComponent()->CharacterStateMapChanged.AddCallback(
		std::bind(&ThisClass::OnCharacterStateMapChanged, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void UEffectsList::OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base* CharacterStatePtr)
{
// 	switch (CharacterStateType)
// 	{
// 	case ECharacterStateType::kActive:
// 	{
// 		auto ItemPtr = AddEffectItem();
// 
// 		EffectItemMap.Add(CharacterStatePtr->GameplayAbilityTargetDataBaseSPtr->Tag, ItemPtr);
// 	}
// 	break;
// 	case ECharacterStateType::kEnd:
// 	{
// 		if (EffectItemMap.Contains(CharacterStatePtr->GameplayAbilityTargetDataBaseSPtr->Tag))
// 		{
// 			if (EffectItemMap[CharacterStatePtr->GameplayAbilityTargetDataBaseSPtr->Tag])
// 			{
// 				EffectItemMap[CharacterStatePtr->GameplayAbilityTargetDataBaseSPtr->Tag]->RemoveFromParent();
// 			}
// 
// 			EffectItemMap.Remove(CharacterStatePtr->GameplayAbilityTargetDataBaseSPtr->Tag);
// 		}
// 	}
// 	break;
// 	default:
// 		break;
// 	}
}

void UEffectsList::OnCharacterStateMapChanged(const TSharedPtr<FCharacterStateInfo>& CharacterStatePtr, bool bIsAdd)
{
	if (bIsAdd)
	{
		auto ItemPtr = AddEffectItem();
		ItemPtr->SetData(CharacterStatePtr);

		EffectItemMap.Add(CharacterStatePtr->Tag, ItemPtr);
	}
	else
	{
		if (EffectItemMap.Contains(CharacterStatePtr->Tag))
		{
			if (EffectItemMap[CharacterStatePtr->Tag])
			{
				EffectItemMap[CharacterStatePtr->Tag]->RemoveFromParent();
			}

			EffectItemMap.Remove(CharacterStatePtr->Tag);
		}
	}
}

void UEffectsList::ResetUIByData()
{
	auto UIPtr = Cast<UMyWrapBox>(GetWidgetFromName(TEXT("WrapBox")));
	if (UIPtr)
	{
		UIPtr->ClearChildren();
	}
}

