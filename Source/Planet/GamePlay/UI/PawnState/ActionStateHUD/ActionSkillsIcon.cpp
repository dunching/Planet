
#include "ActionSkillsIcon.h"

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
#include <CacheAssetManager.h>
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "Skill_Base.h"

namespace ActionSkillsIcon
{
	const FName Percent = TEXT("Percent");

	const FName Content = TEXT("Content");

	const FName StateOverlap = TEXT("StateOverlap");

	const FName Icon = TEXT("Icon");

	const FName CooldownProgress = TEXT("CooldownProgress");

	const FName CooldownText = TEXT("CooldownText");

	const FName CanRelease = TEXT("CanRelease");
}

UActionSkillsIcon::UActionSkillsIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UActionSkillsIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->ToolPtr);
		}
	}
}

void UActionSkillsIcon::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	bIsReady_Previous = false;

	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kSkill)
	{
		ToolPtr = Cast<USkillUnit>(BasicUnitPtr);
	}
	else
	{
		ToolPtr = nullptr;
	}

	SetLevel();
	SetItemType();
	SetCanRelease(true);
	SetRemainingCooldown(true, 0.f, 0.f);
}

void UActionSkillsIcon::EnableIcon(bool bIsEnable)
{

}

void UActionSkillsIcon::UpdateSkillState()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();
	const auto SkillSocketInfoSPtr = EICPtr->FindSkill(IconSocket);
	if (!SkillSocketInfoSPtr)
	{
		return;
	}

	auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
	for (const auto SkillHandleIter : SkillSocketInfoSPtr->HandleAry)
	{
		auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(SkillHandleIter);
		if (!GameplayAbilitySpecPtr)
		{
			return;
		}
		auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
		if (!GAInsPtr)
		{
			return;
		}

		float RemainingCooldown = 0.f;
		float RemainingCooldownPercent = 0.f;

		auto bIsReady = GAInsPtr->CanActivateAbility(GAInsPtr->GetCurrentAbilitySpecHandle(), GAInsPtr->GetCurrentActorInfo());
		SetCanRelease(bIsReady);

		auto bCooldownIsReady = GAInsPtr->GetRemainingCooldown(RemainingCooldown, RemainingCooldownPercent);
		SetRemainingCooldown(bCooldownIsReady, RemainingCooldown, RemainingCooldownPercent);
	}
}

void UActionSkillsIcon::SetLevel()
{
}

void UActionSkillsIcon::SetRemainingCooldown(
	bool bCooldownIsReady,
	float RemainingTime,
	float Percent
)
{
	if (bCooldownIsReady)
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(ActionSkillsIcon::StateOverlap));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(ActionSkillsIcon::StateOverlap));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	{
		auto UIPtr = Cast<UImage>(GetWidgetFromName(ActionSkillsIcon::CooldownProgress));
		if (!UIPtr)
		{
			return;
		}
		auto MIDPtr = UIPtr->GetDynamicMaterial();
		if (MIDPtr)
		{
			MIDPtr->SetScalarParameterValue(ActionSkillsIcon::Percent, Percent);
		}
	}
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(ActionSkillsIcon::CooldownText));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%.1lf"), RemainingTime)));
		}
	}
}

void UActionSkillsIcon::SetCanRelease(bool bIsReady_In)
{
	if (bIsReady_In)
	{
		if (bIsReady_Previous)
		{
			return;
		}
		else
		{
			PlaySkillIsReady();
		}
	}

	bIsReady_Previous = bIsReady_In;

	if (bIsReady_In)
	{
		{
			auto UIPtr = Cast<UImage>(GetWidgetFromName(ActionSkillsIcon::CanRelease));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		{
			auto UIPtr = Cast<UImage>(GetWidgetFromName(ActionSkillsIcon::CanRelease));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UActionSkillsIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(ActionSkillsIcon::Icon));
	if (ImagePtr)
	{
		if (ToolPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(ToolPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
				{
					ImagePtr->SetBrushFromTexture(ToolPtr->GetIcon().Get());
				}));
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UActionSkillsIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

FReply UActionSkillsIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

bool UActionSkillsIcon::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (InOperation->IsA(UItemsDragDropOperation::StaticClass()))
	{
 		auto WidgetDragPtr = Cast<UItemsDragDropOperation>(InOperation);
 		if (WidgetDragPtr)
		{
			auto SkillUnitPtr = Cast<USkillUnit>(WidgetDragPtr->SceneToolSPtr);
			if (SkillUnitPtr && SkillUnitPtr->SkillType == SkillType)
			{
				ResetToolUIByData(WidgetDragPtr->SceneToolSPtr);
			}
 		}
	}

	return true;
}

void UActionSkillsIcon::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	auto BaseItemClassPtr = UAssetRefMap::GetInstance()->DragDropOperationWidgetClass;
	
	if (BaseItemClassPtr)
	{
		auto DragWidgetPtr = CreateWidget<UDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(ToolPtr);

			auto WidgetDragPtr = Cast<UItemsDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemsDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = ToolPtr;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}
