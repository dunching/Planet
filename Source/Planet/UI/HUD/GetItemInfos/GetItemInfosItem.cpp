
#include "GetItemInfosItem.h"

#include "Engine/AssetManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "TemplateHelper.h"

struct FGetItemInfosItem : public TStructVariable<FGetItemInfosItem>
{
	const FName Texture = TEXT("Texture");

	const FName Text = TEXT("Text");
};

void UGetItemInfosItem::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)
{
	PlayThisAnimation();
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FSkillProxy>& UnitPtr, bool bIsAdd)
{
	SetTexutre(UnitPtr->GetIcon());

	SetText(FString::Printf(TEXT("%s %s"), bIsAdd ? TEXT("Get") : TEXT("Lose"), *UnitPtr->GetUnitName()));

	ResetToolUIByData(UnitPtr);
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FConsumableProxy>& UnitPtr, EProxyModifyType ProxyModifyType)
{
	SetTexutre(UnitPtr->GetIcon());

	switch (ProxyModifyType)
	{
	case EProxyModifyType::kAdd:
	{
		SetText(FString::Printf(TEXT("Get:%dX%s"), UnitPtr->GetCurrentValue(), *UnitPtr->GetUnitName()));
	}
		break;
	case EProxyModifyType::kChange:
		break;
	case EProxyModifyType::kRemove:
		break;
	default:
		break;
	}
	ResetToolUIByData(UnitPtr);
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FCoinProxy>& UnitPtr, bool bIsAdd, int32 Num)
{
	SetTexutre(UnitPtr->GetIcon());

	SetText(FString::Printf(TEXT("%s %dX%s"), bIsAdd ? TEXT("Get") : TEXT("Lose"), UnitPtr->GetCurrentValue(), *UnitPtr->GetUnitName()));

	ResetToolUIByData(UnitPtr);
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FCharacterProxy>& UnitPtr, bool bIsAdd)
{
	SetTexutre(UnitPtr->GetIcon());

	SetText(FString::Printf(TEXT("%s X%s"), bIsAdd ? TEXT("Get") : TEXT("Lose"), *UnitPtr->GetUnitName()));

	ResetToolUIByData(UnitPtr);
}

void UGetItemInfosItem::SetTexutre(const TSoftObjectPtr<UTexture2D>& TexturePtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FGetItemInfosItem::Get().Texture));
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
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(FGetItemInfosItem::Get().Text));
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
