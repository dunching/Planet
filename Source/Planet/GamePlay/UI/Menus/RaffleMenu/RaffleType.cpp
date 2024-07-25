
#include "RaffleType.h"

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

namespace RaffleType
{
	FName SelectBtn = TEXT("SelectBtn");

	FName Select_Border = TEXT("Select_Border");

	FName Text = TEXT("Text");

	FName Texture = TEXT("Texture");
}

void URaffleType::NativeConstruct()
{
	Super::NativeConstruct();
	{
		auto UIPtr = Cast<UButton>(GetWidgetFromName(RaffleType::SelectBtn));
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnBtnClicked);
		}
	}
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(RaffleType::Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(DisplayText));
		}
	}
	{
		auto UIPtr = Cast<UImage>(GetWidgetFromName(RaffleType::Texture));
		if (UIPtr)
		{
			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(DefaultIcon.ToSoftObjectPath(), [this, UIPtr]()
				{
					UIPtr->SetBrushFromTexture(DefaultIcon.Get());
				}));
		}
	}
}

void URaffleType::NativeDestruct()
{
	Super::NativeDestruct();
}

void URaffleType::UnSelect()
{
	auto UIPtr = Cast<UImage>(GetWidgetFromName(RaffleType::Select_Border));
	if (UIPtr)
	{
		UIPtr->SetVisibility(ESlateVisibility::Hidden);
	}
}

void URaffleType::Select()
{
	auto UIPtr = Cast<UImage>(GetWidgetFromName(RaffleType::Select_Border));
	if (UIPtr)
	{
		UIPtr->SetVisibility(ESlateVisibility::Visible);
	}

	OnClicked.ExcuteCallback(this);
}

void URaffleType::OnBtnClicked()
{
	Select();
}
 