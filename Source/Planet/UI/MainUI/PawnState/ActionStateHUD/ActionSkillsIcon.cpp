
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

#include "TemplateHelper.h"

#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "CharacterBase.h"
#include "UnitProxyProcessComponent.h"
#include "Skill_Base.h"
#include "GameplayTagsSubSystem.h"
#include "Skill_Active_Base.h"
#include "SceneElement.h"
#include "StateProcessorComponent.h"

struct FActionSkillsIcon : public TStructVariable<FActionSkillsIcon>
{
	const FName Percent = TEXT("Percent");

	const FName Content = TEXT("Content");

	const FName StateOverlap = TEXT("StateOverlap");

	const FName Icon = TEXT("Icon");

	const FName CooldownProgress = TEXT("CooldownProgress");

	const FName CooldownText = TEXT("CooldownText");

	const FName CanRelease = TEXT("CanRelease");

	const FName WaitInputPercent = TEXT("WaitInputPercent");
};

UActionSkillsIcon::UActionSkillsIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UActionSkillsIcon::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InvokeReset(Cast<ThisClass>(ListItemObject));
}

void UActionSkillsIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->UnitPtr);
		}
	}
}

void UActionSkillsIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)
{
	bIsReady_Previous = false;

	UnitPtr = nullptr;

	if (BasicUnitPtr)
	{
		if (
			(BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active)) ||
			(BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon))
			)
		{
			UnitPtr = DynamicCastSharedPtr<FSkillProxy>(BasicUnitPtr);
		}
	}

	SetLevel();
	SetItemType();
	SetCanRelease(true);
	SetRemainingCooldown(true, 0.f, 0.f);

	// 
	SetInputRemainPercent(false, 0.f);
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
	const auto SkillProxySPtr = EICPtr->FindActiveSkillSocket(IconSocket);
	if (!SkillProxySPtr)
	{
		return;
	}

	const auto SKillUnitType = SkillProxySPtr->GetUnitType();
	{
		auto GAInsPtr = SkillProxySPtr->GetGAInst();
		if (!GAInsPtr)
		{
			return;
		}

		auto bIsReady = GAInsPtr->CanActivateAbility(GAInsPtr->GetCurrentAbilitySpecHandle(), GAInsPtr->GetCurrentActorInfo());
		SetCanRelease(bIsReady);
	}
	if (SKillUnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
	{
		float RemainingCooldown = 0.f;
		float RemainingCooldownPercent = 0.f;

		auto ActiveSkillUnitPtr = SkillProxySPtr;
		if (!ActiveSkillUnitPtr)
		{
			return;
		}

		auto bCooldownIsReady = ActiveSkillUnitPtr->GetRemainingCooldown(RemainingCooldown, RemainingCooldownPercent);
		SetRemainingCooldown(bCooldownIsReady, RemainingCooldown, RemainingCooldownPercent);

		bool bIsAcceptInput = false;
		float Percent = 0.f;

		auto GAInsPtr = Cast<USkill_Active_Base>(SkillProxySPtr->GetGAInst());
		if (!GAInsPtr)
		{
			return;
		}

		auto CSSPtr = CharacterPtr->GetStateProcessorComponent()->GetCharacterState(SKillUnitType);
		if (CSSPtr)
		{
			//
			SetDurationPercent(true, CSSPtr->GetRemainTimePercent());
		}
		else
		{
			// 
			GAInsPtr->GetInputRemainPercent(bIsAcceptInput, Percent);
			SetInputRemainPercent(bIsAcceptInput, Percent);
		}
	}
	else if (SKillUnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon))
	{
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
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionSkillsIcon::Get().StateOverlap));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionSkillsIcon::Get().StateOverlap));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	{
		auto UIPtr = Cast<UImage>(GetWidgetFromName(FActionSkillsIcon::Get().CooldownProgress));
		if (!UIPtr)
		{
			return;
		}
		auto MIDPtr = UIPtr->GetDynamicMaterial();
		if (MIDPtr)
		{
			MIDPtr->SetScalarParameterValue(FActionSkillsIcon::Get().Percent, Percent);
		}
	}
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FActionSkillsIcon::Get().CooldownText));
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
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionSkillsIcon::Get().CanRelease));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		{
			auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FActionSkillsIcon::Get().CanRelease));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UActionSkillsIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FActionSkillsIcon::Get().Icon));
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

void UActionSkillsIcon::SetInputRemainPercent(bool bIsAcceptInput, float Percent)
{
	auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(FActionSkillsIcon::Get().WaitInputPercent));
	if (UIPtr)
	{
		UIPtr->SetVisibility(bIsAcceptInput ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		UIPtr->SetPercent(Percent);
	}
}

void UActionSkillsIcon::SetDurationPercent(bool bIsHaveDuration, float Percent)
{
	auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(FActionSkillsIcon::Get().WaitInputPercent));
	if (UIPtr)
	{
		UIPtr->SetVisibility(bIsHaveDuration ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		UIPtr->SetPercent(Percent);
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
			auto OtherUnitPtr = DynamicCastSharedPtr<FSkillProxy>(WidgetDragPtr->SceneToolSPtr);
// 			if (UnitPtr && ProxySPtr->GetUnitType().MatchesTag(SkillUnitType))
// 			{
// 				ResetToolUIByData(WidgetDragPtr->SceneToolSPtr);
// 			}
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
			DragWidgetPtr->ResetToolUIByData(UnitPtr);

			auto WidgetDragPtr = Cast<UItemsDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemsDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = UnitPtr;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}
