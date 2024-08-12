
#include "Raffle_Unit.h"

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
#include "SceneUnitExtendInfo.h"

struct FRaffle_Unit : public TStructVariable<FRaffle_Unit>
{
	FName Texture = TEXT("Texture");

	FName Text = TEXT("Text");
};

void URaffle_Unit::NativeConstruct()
{
	Super::NativeConstruct();
}

void URaffle_Unit::NativeDestruct()
{
	Super::NativeDestruct();
}

void URaffle_Unit::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void URaffle_Unit::ResetToolUIByData(UBasicUnit* BasicUnitPtr)
{
	// 
}

void URaffle_Unit::ResetToolUIByData(FTableRowUnit * TableRowUnitPtr)
{
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FRaffle_Unit::Get().Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(TableRowUnitPtr->UnitName));
		}
	}
	{
		auto UIPtr = Cast<UImage>(GetWidgetFromName(FRaffle_Unit::Get().Texture));
		if (UIPtr)
		{
			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(TableRowUnitPtr->RaffleIcon.ToSoftObjectPath(),
				[TableRowUnitPtr, UIPtr]()
				{
					UIPtr->SetBrushFromTexture(TableRowUnitPtr->RaffleIcon.Get());
				}));
		}
	}
}

void URaffle_Unit::EnableIcon(bool bIsEnable)
{
}
