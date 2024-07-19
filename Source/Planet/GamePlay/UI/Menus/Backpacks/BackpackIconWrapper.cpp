
#include "BackpackIconWrapper.h"

#include <Kismet/GameplayStatics.h>
#include "Containers/UnrealString.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
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
#include "BackpackToolIcon.h"
#include "BackpackConsumableIcon.h"

namespace BackpackIconWrapper
{
	const FName Border = TEXT("Border");
}

UBackpackIconWrapper::UBackpackIconWrapper(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UBackpackIconWrapper::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->TargetBasicUnitPtr);
		}
	}
}

void UBackpackIconWrapper::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	if (BasicUnitPtr)
	{
		switch (BasicUnitPtr->GetSceneToolsType())
		{
		case ESceneToolsType::kTool:
		{
			auto UIPtr = Cast<UBorder>(GetWidgetFromName(BackpackIconWrapper::Border));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}

			auto WidgetPtr = CreateWidget<UBackpackToolIcon>(this, ToolUnitClass);
			if (WidgetPtr)
			{
				UIPtr->AddChild(WidgetPtr);
				WidgetPtr->ResetToolUIByData(BasicUnitPtr);
			}
		}
		break;
		case ESceneToolsType::kConsumables:
		{
			auto UIPtr = Cast<UBorder>(GetWidgetFromName(BackpackIconWrapper::Border));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}

			auto WidgetPtr = CreateWidget<UBackpackConsumableIcon>(this, ConsumableUnitClass);
			if (WidgetPtr)
			{
				UIPtr->AddChild(WidgetPtr);
				WidgetPtr->ResetToolUIByData(BasicUnitPtr);
			}
		}
		break;
		}
	}
}

void UBackpackIconWrapper::EnableIcon(bool bIsEnable)
{

}
