
#include "RaffleBtn.h"

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

struct FRaffleBtn : public TStructVariable<FRaffleBtn>
{
	FName Btn = TEXT("Btn");

	FName Select_Border = TEXT("Select_Border");

	FName Text = TEXT("Text");

	FName Texture = TEXT("Texture");
};

void URaffleBtn::NativeConstruct()
{
	Super::NativeConstruct();
	{
		auto UIPtr = Cast<UButton>(GetWidgetFromName(FRaffleBtn::Get().Btn));
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnBtnClicked);
		}
	}
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FRaffleBtn::Get().Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(DisplayText));
		}
	}
}

void URaffleBtn::NativeDestruct()
{
	Super::NativeDestruct();
}

void URaffleBtn::OnBtnClicked()
{
	OnClicked.ExcuteCallback(this);
}
