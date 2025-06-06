
#include "WeaponsIcon.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CanvasPanel.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Border.h"

#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "BackpackIcon.h"

#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "GameplayTagsLibrary.h"

namespace WeaponsIcon
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

	const FName Icon = TEXT("Icon");
}

UWeaponsIcon::UWeaponsIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UWeaponsIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicProxyPtr)
{
	Super::ResetToolUIByData(InBasicProxyPtr);

	WeaponProxyPtr = nullptr;
	if (BasicProxyPtr && DynamicCastSharedPtr<FBasicProxy>(BasicProxyPtr)->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
	{
		WeaponProxyPtr = DynamicCastSharedPtr<FWeaponProxy>(BasicProxyPtr);
	}

	OnResetProxy_Weapon.ExcuteCallback(WeaponProxyPtr);
}

void UWeaponsIcon::EnableIcon(bool bIsEnable)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(WeaponsIcon::Enable));
	if (ImagePtr)
	{
		ImagePtr->SetVisibility(bIsEnable ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
}

void UWeaponsIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}
