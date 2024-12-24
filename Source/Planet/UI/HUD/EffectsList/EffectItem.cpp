#include "EffectItem.h"

#include "Engine/AssetManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CS_Base.h"
#include "TemplateHelper.h"
#include "CharacterStateInfo.h"

namespace EffectItem
{
	const FName Percent = TEXT("Percent");

	const FName ProgressBar = TEXT("ProgressBar");

	const FName Text = TEXT("Text");

	const FName Icon = TEXT("Icon");

	const FName TextCanvas = TEXT("TextCanvas");
}

void UEffectItem::SetData(const TSharedPtr<FCharacterStateInfo>& InCharacterStateInfoSPtr)
{
	// CharacterStateInfoSPtr = InCharacterStateInfoSPtr;
	//
	// DataChangedHandle = CharacterStateInfoSPtr->DataChanged.AddCallback(std::bind(&ThisClass::OnUpdate, this));
	//
	// SetTexutre(CharacterStateInfoSPtr->DefaultIcon);
	//
	// OnUpdate();
}

void UEffectItem::SetData(const FActiveGameplayEffect* InActiveGameplayEffectPtr)
{
	ActiveGameplayEffectPtr = InActiveGameplayEffectPtr;

	Handle = ActiveGameplayEffectPtr->Handle;
}

void UEffectItem::SetNum(int32 NewNum)
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

void UEffectItem::SetSetNumIsDisplay(bool bIsDisplay)
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

void UEffectItem::SetPercent(bool bIsInversion, float Percent)
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

void UEffectItem::SetPercentIsDisplay(bool bIsDisplay)
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

void UEffectItem::SetTexutre(const TSoftObjectPtr<UTexture2D>& TexturePtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(EffectItem::Icon));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(
			TexturePtr.ToSoftObjectPath(), [this, ImagePtr, TexturePtr]()
			{
				ImagePtr->SetBrushFromTexture(TexturePtr.Get());
			}));
	}
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
		DataChangedHandle->UnBindCallback();
	}

	Super::NativeDestruct();
}

void UEffectItem::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	FGameplayTagContainer OutContainer;
	ActiveGameplayEffectPtr->Spec.GetAllAssetTags(OutContainer);
	if (!OutContainer.IsEmpty())
	{
		// SetTexutre(CharacterStateInfoSPtr->DefaultIcon);

		OnUpdate();
	}
}

void UEffectItem::OnUpdate()
{
	if (ActiveGameplayEffectPtr->Handle != Handle)
	{
		RemoveFromParent();
		return;
	}
	
	SetNum(ActiveGameplayEffectPtr->Spec.GetStackCount());

	switch (ActiveGameplayEffectPtr->Spec.Def->DurationPolicy)
	{
	case EGameplayEffectDurationType::HasDuration:
		{
			const auto Duration = ActiveGameplayEffectPtr->GetDuration();
			const auto TimeRemaining = ActiveGameplayEffectPtr->GetTimeRemaining(GetWorld()->GetTimeSeconds());
			if (TimeRemaining > 0.f)
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
