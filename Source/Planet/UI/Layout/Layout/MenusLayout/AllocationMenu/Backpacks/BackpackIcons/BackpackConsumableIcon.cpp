
#include "BackpackConsumableIcon.h"

#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "ItemProxy_Minimal.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Consumable.h"

struct FBackpackConsumableIcon : public TStructVariable<FBackpackConsumableIcon>
{
	const FName Number = TEXT("Number");
};

UBackpackConsumableIcon::UBackpackConsumableIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UBackpackConsumableIcon::InvokeReset(UUserWidget* InBasicProxyPtr)
{
	if (InBasicProxyPtr)
	{
		auto NewPtr = Cast<ThisClass>(InBasicProxyPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->ProxyPtr);
		}
	}
}

void UBackpackConsumableIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicProxyPtr)
{
	Super::ResetToolUIByData(InBasicProxyPtr);

	if (InBasicProxyPtr && InBasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		ProxyPtr = DynamicCastSharedPtr<FConsumableProxy>(InBasicProxyPtr);
		SetNum(ProxyPtr->GetNum());
	}
}

void UBackpackConsumableIcon::EnableIcon(bool bIsEnable)
{

}

void UBackpackConsumableIcon::SetNum(int32 NewNum)
{
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(FBackpackConsumableIcon::Get().Number));
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

void UBackpackConsumableIcon::SetValue(int32 Value)
{
}

void UBackpackConsumableIcon::ResetSize(const FVector2D& Size)
{
}

void UBackpackConsumableIcon::NativeConstruct()
{
	Super::NativeConstruct();
}
