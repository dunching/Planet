
#include "Raffle_Proxy.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>
#include <Components/Button.h>
#include <Components/Border.h>
#include <Components/Image.h>
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"
#include "TalentIcon.h"
#include "UICommon.h"
#include "SceneProxyExtendInfo.h"

struct FRaffle_Proxy : public TStructVariable<FRaffle_Proxy>
{
	FName Texture = TEXT("Texture");

	FName Text = TEXT("Text");
};

void URaffle_Proxy::NativeConstruct()
{
	Super::NativeConstruct();
}

void URaffle_Proxy::NativeDestruct()
{
	Super::NativeDestruct();
}

void URaffle_Proxy::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void URaffle_Proxy::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)
{
	// 
}

void URaffle_Proxy::ResetToolUIByData(FTableRowProxy * TableRowProxyPtr)
{
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FRaffle_Proxy::Get().Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(TableRowProxyPtr->ProxyName));
		}
	}
	{
		auto UIPtr = Cast<UImage>(GetWidgetFromName(FRaffle_Proxy::Get().Texture));
		if (UIPtr)
		{
			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(TableRowProxyPtr->RaffleIcon.ToSoftObjectPath(),
				[TableRowProxyPtr, UIPtr]()
				{
					UIPtr->SetBrushFromTexture(TableRowProxyPtr->RaffleIcon.Get());
				}));
		}
	}
}

void URaffle_Proxy::EnableIcon(bool bIsEnable)
{
}