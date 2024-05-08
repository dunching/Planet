
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
#include "EquipmentElementComponent.h"
#include "Skill_Base.h"

namespace ActionSkillsIcon
{
	const FName Content = TEXT("Content");

	const FName HighlightBorder = TEXT("HighlightBorder");

	const FName StateOverlap = TEXT("StateOverlap");

	const FName Icon = TEXT("Icon");

	const FName CooldownProgress = TEXT("CooldownProgress");

	const FName CooldownText = TEXT("CooldownText");

	const FName DisableMask = TEXT("DisableMask");
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
			ResetToolUIByData(NewPtr->ToolSPtr);
		}
	}
}

void UActionSkillsIcon::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	bIsReady_Previous = false;

	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kSkill)
	{
		ToolSPtr = Cast<USkillUnit>(BasicUnitPtr);
		SetLevel(ToolSPtr->Level);
		SetItemType();

		{
			auto ImagePtr = Cast<UImage>(GetWidgetFromName(ActionSkillsIcon::HighlightBorder));
			if (ImagePtr)
			{
				ImagePtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		{
			auto BorderPtr = Cast<UBorder>(GetWidgetFromName(ActionSkillsIcon::Content));
			if (BorderPtr)
			{
				BorderPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
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
			auto ImagePtr = Cast<UImage>(GetWidgetFromName(ActionSkillsIcon::HighlightBorder));
			if (ImagePtr)
			{
				ImagePtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
		{
			auto BorderPtr = Cast<UBorder>(GetWidgetFromName(ActionSkillsIcon::Content));
			if (BorderPtr)
			{
				BorderPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(ActionSkillsIcon::StateOverlap));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UActionSkillsIcon::UpdateSkillState()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetEquipmentItemsComponent();
	const auto Ref = EICPtr->FindSkill(IconSocket);

	auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(Ref.Handle);
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

	auto bCooldownIsReady = GAInsPtr->GetRemainingCooldown(RemainingCooldown, RemainingCooldownPercent);

	auto bIsReady = GAInsPtr->CanActivateAbility(GAInsPtr->GetCurrentAbilitySpecHandle(), GAInsPtr->GetCurrentActorInfo());

	SetRemainingCooldown(bIsReady, bCooldownIsReady, RemainingCooldown, RemainingCooldownPercent);
}

void UActionSkillsIcon::SetLevel(int32 NewNum)
{
}

void UActionSkillsIcon::SetRemainingCooldown(
	bool bIsReady_In,
	bool bCooldownIsReady,
	float RemainingTime,
	float Percent
)
{
	bool bIsReady = bIsReady_In && bCooldownIsReady;
	if (bIsReady)
	{
		if (bIsReady_Previous)
		{
			return;
		}
		else
		{
			PlaySkillIsReady();

			{
				auto UIPtr = Cast<UOverlay>(GetWidgetFromName(ActionSkillsIcon::StateOverlap));
				if (UIPtr)
				{
					UIPtr->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
	}

	bIsReady_Previous = bIsReady;

	if (bIsReady_In)
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(ActionSkillsIcon::DisableMask));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(ActionSkillsIcon::DisableMask));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}

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
		{
			auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(ActionSkillsIcon::CooldownProgress));
			if (UIPtr)
			{
				UIPtr->SetPercent(Percent);
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
}

void UActionSkillsIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(Icon));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(ToolSPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
			{
				ImagePtr->SetBrushFromTexture(ToolSPtr->GetIcon().Get());
			});
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
			DragWidgetPtr->ResetToolUIByData(ToolSPtr);

			auto WidgetDragPtr = Cast<UItemsDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemsDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = ToolSPtr;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}
