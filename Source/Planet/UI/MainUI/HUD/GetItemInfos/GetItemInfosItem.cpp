
#include "GetItemInfosItem.h"

#include "Engine/AssetManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

namespace GetItemInfosItem
{
	const FName Texture = TEXT("Texture");

	const FName Text = TEXT("Text");
}

void UGetItemInfosItem::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void UGetItemInfosItem::ResetToolUIByData(UBasicUnit* BasicUnitPtr)
{
	PlayThisAnimation();
}

void UGetItemInfosItem::ResetToolUIByData(USkillUnit* UnitPtr, bool bIsAdd)
{
	SetTexutre(UnitPtr->GetIcon());

	SetText(FString::Printf(TEXT("%s %s"), bIsAdd ? TEXT("Get") : TEXT("Lose"), *UnitPtr->GetUnitName()));

	ResetToolUIByData(UnitPtr);
}

void UGetItemInfosItem::ResetToolUIByData(UConsumableUnit* UnitPtr, bool bIsAdd, int32 Num)
{
	SetTexutre(UnitPtr->GetIcon());

	SetText(FString::Printf(TEXT("%s %dX%s"), bIsAdd ? TEXT("Get") : TEXT("Lose"), UnitPtr->GetCurrentValue(), *UnitPtr->GetUnitName()));

	ResetToolUIByData(UnitPtr);
}

void UGetItemInfosItem::ResetToolUIByData(UCoinUnit* UnitPtr, bool bIsAdd, int32 Num)
{
	SetTexutre(UnitPtr->GetIcon());

	SetText(FString::Printf(TEXT("%s %dX%s"), bIsAdd ? TEXT("Get") : TEXT("Lose"), UnitPtr->GetCurrentValue(), *UnitPtr->GetUnitName()));

	ResetToolUIByData(UnitPtr);
}

void UGetItemInfosItem::ResetToolUIByData(UCharacterUnit* UnitPtr, bool bIsAdd)
{
	SetTexutre(UnitPtr->GetIcon());

	SetText(FString::Printf(TEXT("%s X%s"), bIsAdd ? TEXT("Get") : TEXT("Lose"), *UnitPtr->GetUnitName()));

	ResetToolUIByData(UnitPtr);
}

void UGetItemInfosItem::SetTexutre(const TSoftObjectPtr<UTexture2D>& TexturePtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(GetItemInfosItem::Texture));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(TexturePtr.ToSoftObjectPath(), [this, ImagePtr, TexturePtr]()
			{
				ImagePtr->SetBrushFromTexture(TexturePtr.Get());
			}));
	}
}

void UGetItemInfosItem::SetText(const FString& Text)
{
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(GetItemInfosItem::Text));
	if (!NumTextPtr)
	{
		return;
	}
	NumTextPtr->SetText(FText::FromString(Text));
}

void UGetItemInfosItem::OnAnimationComplete()
{
	RemoveFromParent();
}

void UGetItemInfosItem::EnableIcon(bool bIsEnable)
{
}
