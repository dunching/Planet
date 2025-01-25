
#include "CoinInfo.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>
#include <Components/HorizontalBox.h>
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include <Components/Image.h>

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"
#include "TalentIcon.h"
#include "GameplayTagsLibrary.h"
#include "TemplateHelper.h"

namespace CoinInfo
{
	const FName Number = TEXT("Number");

	const FName Texture = TEXT("Texture");
}

void UCoinInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCoinInfo::NativeDestruct()
{
	if (OnNumChanged)
	{
		OnNumChanged->UnBindCallback();
	}

	Super::NativeDestruct();
}

void UCoinInfo::InvokeReset(UUserWidget* BaseWidgetPtr)
{

}

void UCoinInfo::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)
{
	if (BasicProxyPtr && BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		ProxyPtr = DynamicCastSharedPtr<FCoinProxy>(BasicProxyPtr);

		OnNumChanged = ProxyPtr->CallbackContainerHelper.AddOnValueChanged(std::bind(&ThisClass::SetNum, this, std::placeholders::_2));

		SetNum(ProxyPtr->GetCurrentValue());
		SetItemType();
	}
}

void UCoinInfo::EnableIcon(bool bIsEnable)
{

}

void UCoinInfo::SetNum(int32 NewNum)
{
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(CoinInfo::Number));
	if (!NumTextPtr)
	{
		return;
	}
	if (NewNum >= 0)
	{
		const auto NumStr = FString::Printf(TEXT("%d"), NewNum);

		NumTextPtr->SetText(FText::FromString(NumStr));
	}
	else
	{
		checkNoEntry();
		NumTextPtr->SetText(FText::FromString(TEXT("无效")));
	}
}

void UCoinInfo::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(CoinInfo::Texture));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(ProxyPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
			{
				ImagePtr->SetBrushFromTexture(ProxyPtr->GetIcon().Get());
			}));
	}
}
