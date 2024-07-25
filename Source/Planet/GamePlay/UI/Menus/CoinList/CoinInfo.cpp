
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
	Super::NativeDestruct();
}

void UCoinInfo::InvokeReset(UUserWidget* BaseWidgetPtr)
{

}

void UCoinInfo::ResetToolUIByData(UBasicUnit* BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kCoin)
	{
		UnitPtr = Cast<UCoinUnit>(BasicUnitPtr);
		SetNum(UnitPtr->GetCurrentValue());
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

void UCoinInfo::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(CoinInfo::Texture));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(UnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
			{
				ImagePtr->SetBrushFromTexture(UnitPtr->GetIcon().Get());
			}));
	}
}
