#include "EffectsList.h"

#include "Blueprint\WidgetTree.h"

#include "WrapBox_Override.h"
#include "EffectItem.h"
#include "CharacterBase.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
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

	WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (auto UserWidget = Cast<UWrapBox_Override>(Widget))
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
}

void UEffectsList::NativeDestruct()
{
	Super::NativeDestruct();
}

UEffectItem* UEffectsList::AddEffectItem(
		const TObjectPtr<UAbilitySystemComponent>& AbilitySystemComponentPtr,
		FActiveGameplayEffectHandle NewActiveGameplayEffectHandle
		)
{
	auto ActiveGameplayEffectPtr = AbilitySystemComponentPtr->GetActiveGameplayEffect(NewActiveGameplayEffectHandle);
	if (!ActiveGameplayEffectPtr)
	{
		return nullptr;
	}

	auto UIPtr = Cast<UWrapBox_Override>(GetWidgetFromName(FEffectsList::Get().WrapBox));
	if (UIPtr)
	{
		auto ChildPtr = CreateWidget<UEffectItem>(this, EffectItemClass);
		ChildPtr->SetData(AbilitySystemComponentPtr, NewActiveGameplayEffectHandle);
		
		auto SlotPtr = UIPtr->AddChildToWrapBox(ChildPtr);

		return ChildPtr;
	}

	return nullptr;
}

void UEffectsList::BindCharacterState(ACharacterBase* InTargetCharacterPtr)
{
	TargetCharacterPtr = InTargetCharacterPtr;
}

void UEffectsList::Enable()
{
	ILayoutItemInterfacetion::Enable();
	
	auto UIPtr = Cast<UWrapBox_Override>(GetWidgetFromName(FEffectsList::Get().WrapBox));
	if (UIPtr)
	{
		UIPtr->ClearChildren();
	}

	if (!TargetCharacterPtr)
	{
		return;
	}
	
	CallbackHandle = TargetCharacterPtr->GetStateProcessorComponent()->BindCharacterStateChanged(
		std::bind(&ThisClass::OnCharacterStateChanged, this, std::placeholders::_1, std::placeholders::_2)
	);

	// CharacterStateMapHandle = TargetCharacterPtr->GetStateProcessorComponent()->BindCharacterStateMapChanged(
	// 	std::bind(&ThisClass::OnCharacterStateMapChanged, this, std::placeholders::_1, std::placeholders::_2)
	// );

	if (TargetCharacterPtr && TargetCharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
	{
		auto GASCompPtr = TargetCharacterPtr->GetCharacterAbilitySystemComponent();
		ActiveGameplayEffectHandle = GASCompPtr->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &ThisClass::OnActiveGameplayEffect);
		// AppliedGameplayEffectHandle = GASCompPtr->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::OnActiveGameplayEffect);
	}
}

void UEffectsList::DisEnable()
{
	if (CallbackHandle)
	{
		CallbackHandle->UnBindCallback();
	}

	// if (CharacterStateMapHandle)
	// {
	// 	CharacterStateMapHandle->UnBindCallback();
	// }

#if UE_EDITOR || UE_SERVER
	if (TargetCharacterPtr && TargetCharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
	{
		auto GASCompPtr = TargetCharacterPtr->GetCharacterAbilitySystemComponent();
		GASCompPtr->OnActiveGameplayEffectAddedDelegateToSelf.Remove(ActiveGameplayEffectHandle);
		GASCompPtr->OnActiveGameplayEffectAddedDelegateToSelf.Remove(AppliedGameplayEffectHandle);
	}
#endif
	
	ILayoutItemInterfacetion::DisEnable();
}

void UEffectsList::OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base* CharacterStatePtr)
{
}

void UEffectsList::OnCharacterStateMapChanged(const TSharedPtr<FCharacterStateInfo>& CharacterStatePtr, bool bIsAdd)
{
	// if (bIsAdd)
	// {
	// 	auto ItemPtr = AddEffectItem();
	// 	ItemPtr->SetData(CharacterStatePtr);
	//
	// 	EffectItemMap.Add(CharacterStatePtr->Guid, ItemPtr);
	// }
	// else
	// {
	// 	if (EffectItemMap.Contains(CharacterStatePtr->Guid))
	// 	{
	// 		if (EffectItemMap[CharacterStatePtr->Guid])
	// 		{
	// 			EffectItemMap[CharacterStatePtr->Guid]->RemoveFromParent();
	// 		}
	//
	// 		EffectItemMap.Remove(CharacterStatePtr->Guid);
	// 	}
	// }
}

void UEffectsList::OnActiveGameplayEffect(
	UAbilitySystemComponent *AbilitySystemComponentPtr,
	const FGameplayEffectSpec&GameplayEffectSpec,
	FActiveGameplayEffectHandle InActiveGameplayEffectHandle
	)
{
	auto ActiveGameplayEffectPtr = AbilitySystemComponentPtr->GetActiveGameplayEffect(InActiveGameplayEffectHandle);

	FGameplayTagContainer OutContainer;
	GameplayEffectSpec.GetAllAssetTags(OutContainer);

	if (OutContainer.HasTag(UGameplayTagsLibrary::GEData_Info))
	{
		auto ItemPtr = AddEffectItem(AbilitySystemComponentPtr, InActiveGameplayEffectHandle);
	}
}