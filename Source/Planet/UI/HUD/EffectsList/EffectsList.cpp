
#include "EffectsList.h"

#include "Blueprint\WidgetTree.h"

#include "MyWrapBox.h"
#include "EffectItem.h"
#include "CS_Base.h"
#include "CharacterBase.h"
#include "BaseFeatureComponent.h"
#include "StateProcessorComponent.h"
#include "TemplateHelper.h"

struct FEffectsList : public TStructVariable<FEffectsList>
{
	const FName WrapBox = TEXT("WrapBox");
};

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void UEffectsList::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	WidgetTree->ForEachWidget([&](UWidget* Widget) {
		if (auto UserWidget = Cast<UMyWrapBox>(Widget))
		{
			UserWidget->bIsPositiveSequence = bIsPositiveSequence;
			UserWidget->SynchronizeProperties();
		}
		});
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

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
	auto UIPtr = Cast<UMyWrapBox>(GetWidgetFromName(FEffectsList::Get().WrapBox));
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
}

void UEffectsList::OnCharacterStateMapChanged(const TSharedPtr<FCharacterStateInfo>& CharacterStatePtr, bool bIsAdd)
{
	if (bIsAdd)
	{
		auto ItemPtr = AddEffectItem();
		ItemPtr->SetData(CharacterStatePtr);

		EffectItemMap.Add(CharacterStatePtr->Guid, ItemPtr);
	}
	else
	{
		if (EffectItemMap.Contains(CharacterStatePtr->Guid))
		{
			if (EffectItemMap[CharacterStatePtr->Guid])
			{
				EffectItemMap[CharacterStatePtr->Guid]->RemoveFromParent();
			}

			EffectItemMap.Remove(CharacterStatePtr->Guid);
		}
	}
}

void UEffectsList::ResetUIByData()
{
	auto UIPtr = Cast<UMyWrapBox>(GetWidgetFromName(FEffectsList::Get().WrapBox));
	if (UIPtr)
	{
		UIPtr->ClearChildren();
	}
}

