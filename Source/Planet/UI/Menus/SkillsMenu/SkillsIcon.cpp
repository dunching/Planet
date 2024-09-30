
#include "SkillsIcon.h"

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
#include "ProxyProcessComponent.h"
#include "Skill_Base.h"
#include "GameplayTagsSubSystem.h"

namespace SkillsIcon
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

	const FName Icon = TEXT("Icon");
}

USkillsIcon::USkillsIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void USkillsIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicUnitPtr)
{
	Super::ResetToolUIByData(InBasicUnitPtr);

	SetLevel();
}

void USkillsIcon::EnableIcon(bool bIsEnable)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(SkillsIcon::Enable));
	if (ImagePtr)
	{
		ImagePtr->SetVisibility(bIsEnable ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
}

void USkillsIcon::SetLevel()
{
}

void USkillsIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

void USkillsIcon::NativeDestruct()
{
	Super::NativeDestruct();
}
