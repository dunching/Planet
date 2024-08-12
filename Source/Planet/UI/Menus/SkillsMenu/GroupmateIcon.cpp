
#include "GroupmateIcon.h"

#include <Components/Button.h>
#include <Components/Image.h>
#include <Components/Border.h>
#include <Components/TextBlock.h>
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "UICommon.h"
#include "CharacterAttributesComponent.h"
#include "SceneElement.h"
#include "CharacterAttibutes.h"

struct FGroupmateIcon : public TStructVariable<FGroupmateIcon>
{
	const FName SelectBtn = TEXT("SelectBtn");

	const FName Selected = TEXT("Selected");

	const FName Icon = TEXT("Icon");

	const FName Text = TEXT("Text");
};

void UGroupmateIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{

}

void UGroupmateIcon::ResetToolUIByData(UBasicUnit* InBasicUnitPtr)
{
	UnitPtr = nullptr;
	UnitPtr = Cast<UCharacterUnit>(InBasicUnitPtr);

	SetItemType();
	SetName();
}

void UGroupmateIcon::EnableIcon(bool bIsEnable)
{

}

void UGroupmateIcon::SwitchSelectState(bool bIsSelect)
{
	auto UIPtr = Cast<UImage>(GetWidgetFromName(FGroupmateIcon::Get().Selected));
	if (UIPtr)
	{
		UIPtr->SetVisibility(bIsSelect ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UGroupmateIcon::NativeConstruct()
{
	Super::NativeConstruct();

	auto BtnPtr = Cast<UButton>(GetWidgetFromName(FGroupmateIcon::Get().SelectBtn));
	if (BtnPtr)
	{
		BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnBtnCliked);
	}

	SwitchSelectState(false);
}

void UGroupmateIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FGroupmateIcon::Get().Icon));
	if (ImagePtr)
	{
		if (UnitPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(UnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
				{
					ImagePtr->SetBrushFromTexture(UnitPtr->GetIcon().Get());
				}));
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UGroupmateIcon::SetName()
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FGroupmateIcon::Get().Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromName(UnitPtr->CharacterAttributes->Name));
	}
}

void UGroupmateIcon::OnBtnCliked()
{
	SwitchSelectState(true);
	OnSelected.ExcuteCallback(UnitPtr);
}
