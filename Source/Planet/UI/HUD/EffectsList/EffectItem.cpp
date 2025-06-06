#include "EffectItem.h"

#include "AbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameplayEffect.h"

#include "TemplateHelper.h"
#include "CharacterStateInfo.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Description.h"
#include "DataTableCollection.h"
#include "StateTagExtendInfo.h"

namespace EffectItem
{
	const FName Percent = TEXT("Percent");

	const FName ProgressBar = TEXT("ProgressBar");

	const FName Text = TEXT("Text");

	const FName Icon = TEXT("Icon");

	const FName TextCanvas = TEXT("TextCanvas");
}

void UEffectItem::SetNum(
	int32 NewNum
)
{
	SetSetNumIsDisplay(true);
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(EffectItem::Text));
	if (!NumTextPtr)
	{
		return;
	}
	if (NewNum > 0)
	{
		const auto NumStr = FString::Printf(TEXT("%d"), NewNum);

		NumTextPtr->SetText(FText::FromString(NumStr));
	}
	else
	{
		NumTextPtr->SetText(FText::FromString(TEXT("")));
	}
}

void UEffectItem::SetSetNumIsDisplay(
	bool bIsDisplay
)
{
	{
		auto UIPtr = GetWidgetFromName(EffectItem::TextCanvas);
		if (!UIPtr)
		{
			return;
		}
		UIPtr->SetVisibility(bIsDisplay ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UEffectItem::SetPercent(
	bool bIsInversion,
	float Percent
)
{
	SetPercentIsDisplay(true);
	auto UIPtr = Cast<UImage>(GetWidgetFromName(EffectItem::ProgressBar));
	if (!UIPtr)
	{
		return;
	}
	auto MIDPtr = UIPtr->GetDynamicMaterial();
	if (MIDPtr)
	{
		MIDPtr->SetScalarParameterValue(EffectItem::Percent, Percent);
	}
}

void UEffectItem::SetPercentIsDisplay(
	bool bIsDisplay
)
{
	{
		auto UIPtr = GetWidgetFromName(EffectItem::ProgressBar);
		if (!UIPtr)
		{
			return;
		}
		UIPtr->SetVisibility(bIsDisplay ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UEffectItem::SetTexutre(
)
{
	auto ActiveGameplayEffectPtr = AbilitySystemComponentPtr->GetActiveGameplayEffect(Handle);
	if (!ActiveGameplayEffectPtr)
	{
		return;
	}

	auto ImagePtr = Cast<UImage>(GetWidgetFromName(EffectItem::Icon));
	if (!ImagePtr)
	{
		return;
	}

	FGameplayTagContainer OutContainer;
	ActiveGameplayEffectPtr->Spec.GetAllAssetTags(OutContainer);

	for (const auto& Iter : OutContainer)
	{
		if (Iter.MatchesTag(UGameplayTagsLibrary::Proxy_Skill))
		{
			auto SceneProxyExtendInfoPtr = GetTableRowProxy(Iter);
			if (!SceneProxyExtendInfoPtr)
			{
				return;
			}

			AsyncLoadText(SceneProxyExtendInfoPtr->ItemProxy_Description.LoadSynchronous()->DefaultIcon, ImagePtr);

			return;
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
		{
			auto SceneProxyExtendInfoPtr = GetTableRowProxy(Iter);
			if (!SceneProxyExtendInfoPtr)
			{
				return;
			}

			AsyncLoadText(SceneProxyExtendInfoPtr->ItemProxy_Description.LoadSynchronous()->DefaultIcon, ImagePtr);

			return;
		}
	}
}

void UEffectItem::SetData(
	const TObjectPtr<UAbilitySystemComponent>& InAbilitySystemComponentPtr,
	FActiveGameplayEffectHandle NewActiveGameplayEffectHandle
)
{
	AbilitySystemComponentPtr = InAbilitySystemComponentPtr;
	Handle = NewActiveGameplayEffectHandle;

	SetTexutre();
}

void UEffectItem::NativeConstruct()
{
	Super::NativeConstruct();

	SetSetNumIsDisplay(false);
	SetPercentIsDisplay(false);
}

void UEffectItem::NativeDestruct()
{
	if (DataChangedHandle)
	{
		DataChangedHandle.Reset();
	}

	Super::NativeDestruct();
}

void UEffectItem::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	auto ActiveGameplayEffectPtr = AbilitySystemComponentPtr->GetActiveGameplayEffect(Handle);
	if (!ActiveGameplayEffectPtr)
	{
		RemoveFromParent();
		return;
	}

	if (!ActiveGameplayEffectPtr)
	{
		return;
	}
	FGameplayTagContainer OutContainer;
	ActiveGameplayEffectPtr->Spec.GetAllAssetTags(OutContainer);
	if (!OutContainer.IsEmpty())
	{
		OnUpdate();
	}
}

void UEffectItem::OnUpdate()
{
	auto ActiveGameplayEffectPtr = AbilitySystemComponentPtr->GetActiveGameplayEffect(Handle);
	if (!ActiveGameplayEffectPtr)
	{
		return;
	}

	if (ActiveGameplayEffectPtr->Handle != Handle)
	{
		RemoveFromParent();
		return;
	}

	const auto Count = ActiveGameplayEffectPtr->Spec.GetStackCount();
	SetNum(Count);

	switch (ActiveGameplayEffectPtr->Spec.Def->DurationPolicy)
	{
	case EGameplayEffectDurationType::HasDuration:
		{
			const auto Duration = ActiveGameplayEffectPtr->GetDuration();
			const auto TimeRemaining = ActiveGameplayEffectPtr->GetTimeRemaining(GetWorld()->GetTimeSeconds());
			if (TimeRemaining > 0.f || Count > 1)
			{
				SetPercent(true, TimeRemaining / Duration);
			}
			else
			{
				RemoveFromParent();
			}
		}
		break;
	default:
		{
			// 没写
			const auto TimeRemaining = ActiveGameplayEffectPtr->GetTimeRemaining(GetWorld()->GetTimeSeconds());
			if (TimeRemaining > 0.f)
			{
				SetPercent(true, TimeRemaining);
			}
			else
			{
				SetPercentIsDisplay(false);
			}
		}
		break;
	}
}
