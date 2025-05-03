
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

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FSkillProxy>& ProxyPtr, EProxyModifyType ProxyModifyType)
{
	SetTexutre(ProxyPtr->GetIcon());

	const auto Text =
		FString::Printf(
			TEXT("%s:%s"),
			ProxyModifyType == EProxyModifyType::kNumChanged ? * UTextSubSystem::GetInstance()->GetText(TextCollect::GetSkill) : *UTextSubSystem::GetInstance()->GetText(TextCollect::LoseSkill),
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
	case EProxyModifyType::kNumChanged:
		{
			if (ProxyPtr->GetOffsetNum() > 0)
			{
				SetText(FString::Printf(
					TEXT("%s %dX%s"), TEXT("Get"), ProxyPtr->GetOffsetNum(), *ProxyPtr->GetProxyName()
					));
			}
			else
			{
				SetText(FString::Printf(
					TEXT("%s %dX%s"), TEXT("Lose"), ProxyPtr->GetOffsetNum(), *ProxyPtr->GetProxyName()
					));
			}
	}
	break;
	case EProxyModifyType::kPropertyChange:
		break;
	case EProxyModifyType::kRemove:
		{
			SetText(FString::Printf(TEXT("Lose:%dX%s"), ProxyPtr->GetNum(), *ProxyPtr->GetProxyName()));
		}
		break;
	default:
		break;
	}
	
	ResetToolUIByData(ProxyPtr);
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FCoinProxy>& ProxyPtr, EProxyModifyType ProxyModifyType, int32 Num)
{
	SetTexutre(ProxyPtr->GetIcon());

	switch (ProxyModifyType)
	{
	case EProxyModifyType::kNumChanged:
		{
			if (ProxyPtr->GetOffsetNum() > 0)
			{
				SetText(FString::Printf(
					TEXT("%s %dX%s"), TEXT("Get"), ProxyPtr->GetOffsetNum(), *ProxyPtr->GetProxyName()
					));
			}
			else
			{
				SetText(FString::Printf(
					TEXT("%s %dX%s"), TEXT("Lose"), ProxyPtr->GetOffsetNum(), *ProxyPtr->GetProxyName()
					));
			}
		}
		break;
	case EProxyModifyType::kPropertyChange:
		break;
	case EProxyModifyType::kRemove:
		{
		}
		break;
	default:
		break;
	}
	
	ResetToolUIByData(ProxyPtr);
}

void UGetItemInfosItem::ResetToolUIByData(const TSharedPtr < FCharacterProxy>& ProxyPtr, EProxyModifyType ProxyModifyType)
{
	SetTexutre(ProxyPtr->GetIcon());

	SetText(FString::Printf(
		TEXT("%s X%s"), ProxyModifyType == EProxyModifyType::kNumChanged ? TEXT("Get") : TEXT("Lose"), *ProxyPtr->GetProxyName()
		));

	ResetToolUIByData(ProxyPtr);
}

void UGetItemInfosItem::SetTexutre(const TSoftObjectPtr<UTexture2D>& TexturePtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FGetItemInfosItem::Get().Texture));
	if (ImagePtr)
	{
			AsyncLoadText(TexturePtr,ImagePtr );
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
