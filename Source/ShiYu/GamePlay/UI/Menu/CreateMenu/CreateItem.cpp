
#include "CreateItem.h"

#include <Kismet/GameplayStatics.h>

#include "Components/Image.h"
#include "Components/Button.h"
#include "PlayerState/ShiYuPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Components/TextBlock.h"
#include "ToolsLibrary.h"

#include <CacheAssetManager.h>

UCreateItem::UCreateItem(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void UCreateItem::NativeConstruct()
{
	Super::NativeConstruct();

	auto BtnPtr = Cast<UButton>(GetWidgetFromName(TEXT("CreateBtn")));
	if (BtnPtr)
	{
		BtnPtr->OnClicked.AddDynamic(this, &UCreateItem::OnCreateBtnClick);
	}

	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Image")));
	if (ImagePtr)
	{
		auto CacheAssetManagerPtr = UCacheAssetManager::GetInstance();

		CacheAssetManagerPtr->GetTextureByItemType(ItemsType, [ImagePtr](auto TexturePtr)
			{
			ImagePtr->SetBrushFromTexture(TexturePtr);
			});
	}
}

void UCreateItem::ResetWidget(UCreateItem* TargetUIPtr)
{
	if (!TargetUIPtr)
	{
		return;
	}

	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Image")));
	if (ImagePtr)
	{
		auto CacheAssetManagerPtr = UCacheAssetManager::GetInstance();
		CacheAssetManagerPtr->GetTextureByItemType(TargetUIPtr->ItemsType, [ImagePtr](auto TexturePtr)
			{
			ImagePtr->SetBrushFromTexture(TexturePtr);
			});
	}

	auto TextPtr = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
	if (TextPtr)
	{
		auto RightTextPtr = Cast<UTextBlock>(TargetUIPtr->GetWidgetFromName(TEXT("Text")));
		TextPtr->SetText(RightTextPtr->GetText());
	}

	auto BtnPtr = Cast<UButton>(GetWidgetFromName(TEXT("CreateBtn")));
	if (BtnPtr)
	{
		auto RightBtnPtr = Cast<UButton>(TargetUIPtr->GetWidgetFromName(TEXT("CreateBtn")));
		BtnPtr->SetIsEnabled(RightBtnPtr->GetIsEnabled());
	}

	ItemsType = TargetUIPtr->ItemsType;
	OnCreateItem = TargetUIPtr->OnCreateItem;
}

void UCreateItem::IsAbleCreateItem(bool bIsAbleCreate)
{
	auto BtnPtr = Cast<UButton>(GetWidgetFromName(TEXT("CreateBtn")));
	if (BtnPtr)
	{
		BtnPtr->SetIsEnabled(bIsAbleCreate);
	}
	auto TextPtr = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
	if (TextPtr)
	{
		TextPtr->SetText(FText::FromString(bIsAbleCreate ? TEXT("Create") : TEXT("Cant Create")));
	}
}

void UCreateItem::SetOnCreateItem(const FOnCreateItem& NewOnCreateItem)
{
	OnCreateItem = NewOnCreateItem;
}

void UCreateItem::OnCreateBtnClick()
{
	if (OnCreateItem)
	{
		OnCreateItem(ItemsType);
	}
}
