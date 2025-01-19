
#include "GetItemInfosItem.h"

#include "Engine/AssetManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "TemplateHelper.h"
#include "TextSubSystem.h"
#include "TextCollect.h"
#include "ItemProxy_Character.h"

struct FGetItemInfosItem : public TStructVariable<FGetItemInfosItem>
{
	const FName Texture = TEXT("Texture");

	const FName Text = TEXT("Text");
};

void UGetItemInfosItem::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)
{
	PlayThisAnimation();
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FSkillProxy>& ProxyPtr, bool bIsAdd)
{
	SetTexutre(ProxyPtr->GetIcon());

	const auto Text =
		FString::Printf(
			TEXT("%s:%s"),
			bIsAdd ? * UTextSubSystem::GetInstance()->GetText(TextCollect::GetSkill) : *UTextSubSystem::GetInstance()->GetText(TextCollect::LoseSkill),
			*ProxyPtr->GetProxyName()
		);
	SetText(Text);

	ResetToolUIByData(ProxyPtr);
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FConsumableProxy>& ProxyPtr, EProxyModifyType ProxyModifyType)
{
	SetTexutre(ProxyPtr->GetIcon());

	switch (ProxyModifyType)
	{
	case EProxyModifyType::kAdd:
	{
		SetText(FString::Printf(TEXT("Get:%dX%s"), ProxyPtr->GetCurrentValue(), *ProxyPtr->GetProxyName()));
	}
	break;
	case EProxyModifyType::kChange:
		break;
	case EProxyModifyType::kRemove:
		break;
	default:
		break;
	}
	ResetToolUIByData(ProxyPtr);
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FCoinProxy>& ProxyPtr, bool bIsAdd, int32 Num)
{
	SetTexutre(ProxyPtr->GetIcon());

	SetText(FString::Printf(TEXT("%s %dX%s"), bIsAdd ? TEXT("Get") : TEXT("Lose"), ProxyPtr->GetCurrentValue(), *ProxyPtr->GetProxyName()));

	ResetToolUIByData(ProxyPtr);
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FCharacterProxy>& ProxyPtr, bool bIsAdd)
{
	SetTexutre(ProxyPtr->GetIcon());

	SetText(FString::Printf(TEXT("%s X%s"), bIsAdd ? TEXT("Get") : TEXT("Lose"), *ProxyPtr->GetProxyName()));

	ResetToolUIByData(ProxyPtr);
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
	OnFinished.ExecuteIfBound();
}

void UGetItemInfosItem::EnableIcon(bool bIsEnable)
{
}
