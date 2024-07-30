
#include "ConsumableIcon.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CanvasPanel.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetStringLibrary.h"

#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "BackpackIcon.h"

#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "Skill_Base.h"
#include "GameplayTagsSubSystem.h"
#include "UICommon.h"

struct FConsumableIcon : public TGetSocketName<FConsumableIcon>
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

	const FName Icon = TEXT("Icon");
};

UConsumableIcon::UConsumableIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UConsumableIcon::ResetToolUIByData(UBasicUnit* InBasicUnitPtr)
{
	Super::ResetToolUIByData(InBasicUnitPtr);

	UnitPtr = nullptr;
	if (BasicUnitPtr && (
		BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables)
		))
	{
		UnitPtr = Cast<UConsumableUnit>(BasicUnitPtr);
	}

	OnResetUnit.ExcuteCallback(UnitPtr);
	SetLevel();
}

void UConsumableIcon::EnableIcon(bool bIsEnable)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FConsumableIcon::Get().Enable));
	if (ImagePtr)
	{
		ImagePtr->SetVisibility(bIsEnable ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
}

void UConsumableIcon::SetLevel()
{
}

void UConsumableIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

void UConsumableIcon::NativeDestruct()
{
	Super::NativeDestruct();
}
