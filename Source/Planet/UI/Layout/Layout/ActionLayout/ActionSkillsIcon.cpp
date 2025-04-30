
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
#include "Kismet/KismetStringLibrary.h"

#include "TemplateHelper.h"

#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Skill_Base.h"
#include "GameplayTagsLibrary.h"
#include "Skill_Active_Base.h"
#include "Skill_WeaponActive_Base.h"
#include "ItemProxy_Minimal.h"
#include "StateProcessorComponent.h"

struct FActionSkillsIcon : public TStructVariable<FActionSkillsIcon>
{
	const FName Percent = TEXT("Percent");

	const FName Content = TEXT("Content");

	const FName StateOverlap = TEXT("StateOverlap");

	const FName Icon = TEXT("Icon");

	const FName Number = TEXT("Number");

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
			ResetToolUIByData(NewPtr->ProxyPtr);
		}
	}
}

void UActionSkillsIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)
{
	bIsReady_Previous = false;

	ProxyPtr = nullptr;

	if (BasicProxyPtr)
	{
		if (
			(BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active)) ||
			(BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
			)
		{
			ProxyPtr = DynamicCastSharedPtr<FSkillProxy>(BasicProxyPtr);
		}
	}

	SetLevel();
	SetItemType();
	SetCanRelease(true);
	SetRemainingCooldown(true, 0.f, 0.f);

	// 
	SetInputRemainPercent(false, 0.f);
	SetNum(false, 0);
}

void UActionSkillsIcon::EnableIcon(bool bIsEnable)
{

}

void UActionSkillsIcon::UpdateSkillState()
{
	if (IconSocket.MatchesTag(UGameplayTagsLibrary::ActiveSocket))
	{
		UpdateSkillState_ActiveSkill();
	}
	else if (IconSocket.MatchesTag(UGameplayTagsLibrary::WeaponSocket))
	{
		UpdateSkillState_ActiveWeapon();
	}
}

void UActionSkillsIcon::UpdateSkillState_ActiveSkill()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	const auto SKillProxyType = ProxyPtr->GetProxyType();
	{
		auto GAInsPtr = ProxyPtr->GetGAInst();
		if (!GAInsPtr)
		{
			return;
		}

		if (GAInsPtr->IsActive())
		{
			const auto RemainTime = GAInsPtr->GetRemainTime();

			SetDurationPercent(true, RemainTime < 0.f ? 1.f : RemainTime);
			return;
		}
		
		auto bIsReady = GAInsPtr->CanActivateAbility(GAInsPtr->GetCurrentAbilitySpecHandle(), GAInsPtr->GetCurrentActorInfo());
		SetCanRelease(bIsReady);
	}
	if (SKillProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
	{
		SetDurationPercent(false, 0.f);
		const auto SkillProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(ProxyPtr);
		if (!SkillProxySPtr)
		{
			return;
		}

		float RemainingCooldown = 0.f;
		float RemainingCooldownPercent = 0.f;

		auto ActiveSkillProxyPtr = SkillProxySPtr;
		if (!ActiveSkillProxyPtr)
		{
			return;
		}

		auto bCooldownIsReady = ActiveSkillProxyPtr->GetRemainingCooldown(RemainingCooldown, RemainingCooldownPercent);
		SetRemainingCooldown(bCooldownIsReady, RemainingCooldown, RemainingCooldownPercent);

		bool bIsAcceptInput = false;
		float Percent = 0.f;

		auto GAInsPtr = Cast<USkill_Active_Base>(SkillProxySPtr->GetGAInst());
		if (!GAInsPtr)
		{
			return;
		}
	}
}

void UActionSkillsIcon::UpdateSkillState_ActiveWeapon()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	const auto SkillProxySPtr = ProxyPtr;
	if (!SkillProxySPtr)
	{
		return;
	}

	const auto SKillProxyType = SkillProxySPtr->GetProxyType();
	{
		auto GAInsPtr = SkillProxySPtr->GetGAInst();
		if (!GAInsPtr)
		{
			return;
		}
		auto bIsReady = GAInsPtr->CanActivateAbility(GAInsPtr->GetCurrentAbilitySpecHandle(), GAInsPtr->GetCurrentActorInfo());
		SetCanRelease(bIsReady);
	}

	if (SKillProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
	{
		auto GAInsPtr = Cast<USkill_WeaponActive_Base>(SkillProxySPtr->GetGAInst());
		if (!GAInsPtr)
		{
			return;
		}

		int32 Num = 0;
		const auto bIsDisplay = GAInsPtr->GetNum(Num);
		SetNum(bIsDisplay, Num);
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
				return;
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
		if (ProxyPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			AsyncLoadText(ProxyPtr->GetIcon(),ImagePtr );
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UActionSkillsIcon::SetNum(bool bIsDisplay, int32 Num)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FActionSkillsIcon::Get().Number));
	if (UIPtr)
	{
		UIPtr->SetVisibility(bIsDisplay ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		if (bIsDisplay)
		{
			UIPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Num)));
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

	if (InOperation->IsA(UItemProxyDragDropOperation::StaticClass()))
	{
 		auto WidgetDragPtr = Cast<UItemProxyDragDropOperation>(InOperation);
 		if (WidgetDragPtr)
		{
			auto OtherProxyPtr = DynamicCastSharedPtr<FSkillProxy>(WidgetDragPtr->SceneToolSPtr);
// 			if (ProxyPtr && ProxySPtr->GetProxyType().MatchesTag(SkillProxyType))
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
		auto DragWidgetPtr = CreateWidget<UItemProxyDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(ProxyPtr);

			auto WidgetDragPtr = Cast<UItemProxyDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemProxyDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = ProxyPtr;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}
