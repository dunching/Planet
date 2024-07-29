
#include "BackpackIcon.h"

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
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "SceneElement.h"
#include "GameplayTagsSubSystem.h"

UBackpackIcon::UBackpackIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UBackpackIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
		}
	}
}

void UBackpackIcon::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	if (BasicUnitPtr && UGameplayTagsSubSystem::GetInstance()->Unit_Tool.MatchesTag(BasicUnitPtr->GetUnitType()))
	{
	}
}

void UBackpackIcon::EnableIcon(bool bIsEnable)
{

}
