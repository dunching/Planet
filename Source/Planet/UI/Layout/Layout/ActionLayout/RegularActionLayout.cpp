#include "RegularActionLayout.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Border.h"

#include "TemplateHelper.h"
#include "PlanetPlayerController.h"
#include "UIManagerSubSystem.h"
#include "EffectsList.h"
#include "CharacterBase.h"
#include "FocusIcon.h"
#include "AssetRefMap.h"
#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "UICommon.h"
#include "FocusTitle.h"
#include "GameOptions.h"
#include "GroupManagger.h"
#include "GuideActor.h"
#include "GuideList.h"
#include "GuideSubSystem.h"
#include "HUD_TeamInfo.h"
#include "HumanCharacter_Player.h"
#include "PawnStateActionHUD.h"
#include "ProgressTips.h"
#include "TeamMatesHelperComponent.h"

struct FRegularActionLayout : public TStructVariable<FRegularActionLayout>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnStateActionHUD = TEXT("PawnStateActionHUD");

	FName FocusCharacterSocket = TEXT("FocusCharacterSocket");

	FName GuideList = TEXT("GuideList");

	FName LowerHPSocket = TEXT("LowerHPSocket");

	FName HUD_TeamInfoSocket = TEXT("HUD_TeamInfoSocket");
};

void URegularActionLayout::NativeConstruct()
{
	Super::NativeConstruct();

	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacterPtr)
	{
		DisplayTeamInfo(true);
		InitialEffectsList();
		ViewProgressTips(false);

		OnFocusCharacter(nullptr);
		auto DelegateHandle =
			PlayerCharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->OnFocusCharacterDelegate.
			                    AddCallback(
				                    std::bind(&ThisClass::OnFocusCharacter, this, std::placeholders::_1)
			                    );
		DelegateHandle->bIsAutoUnregister = false;

		auto CharacterAttributesRef =
			PlayerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

		PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
			CharacterAttributesRef->GetMax_HPAttribute()
		).AddUObject(this, &ThisClass::OnHPChanged);

		PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
			CharacterAttributesRef->GetHPAttribute()
		).AddUObject(this, &ThisClass::OnHPChanged);
	}
}

void URegularActionLayout::Enable()
{
	ILayoutInterfacetion::Enable();

	{
		auto UIPtr = Cast<UPawnStateActionHUD>(GetWidgetFromName(FRegularActionLayout::Get().PawnStateActionHUD));
		if (UIPtr)
		{
			UIPtr->Enable();
			UIPtr->ResetUIByData();
		}
	}
}

void URegularActionLayout::DisEnable()
{
	{
		auto UIPtr = Cast<UPawnStateActionHUD>(GetWidgetFromName(FRegularActionLayout::Get().PawnStateActionHUD));
		if (UIPtr)
		{
			UIPtr->DisEnable();
		}
	}

	ILayoutInterfacetion::DisEnable();
}

void URegularActionLayout::OnFocusCharacter(
	ACharacterBase* TargetCharacterPtr
)
{
	// 
	if (TargetCharacterPtr)
	{
		auto AssetRefMapPtr = UAssetRefMap::GetInstance();
		FocusIconPtr = CreateWidget<UFocusIcon>(GetWorldImp(), AssetRefMapPtr->FocusIconClass);
		if (FocusIconPtr)
		{
			FocusIconPtr->TargetCharacterPtr = TargetCharacterPtr;
			FocusIconPtr->AddToViewport(EUIOrder::kFocus);
		}
	}
	else
	{
		if (FocusIconPtr)
		{
			FocusIconPtr->RemoveFromParent();
			FocusIconPtr = nullptr;
		}
	}

	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FRegularActionLayout::Get().FocusCharacterSocket));
	if (!BorderPtr)
	{
		return;
	}

	if (TargetCharacterPtr)
	{
		// 
		UFocusTitle* UIPtr = nullptr;
		for (auto Iter : BorderPtr->GetAllChildren())
		{
			UIPtr = Cast<UFocusTitle>(Iter);
			if (UIPtr)
			{
				break;
			}
		}
		if (!UIPtr)
		{
			UIPtr = CreateWidget<UFocusTitle>(GetWorldImp(), FocusTitleClass);
			BorderPtr->AddChild(UIPtr);
		}
		if (UIPtr)
		{
			UIPtr->SetTargetCharacter(TargetCharacterPtr);
		}
	}
	else
	{
		BorderPtr->ClearChildren();
	}
}

UEffectsList* URegularActionLayout::InitialEffectsList()
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(EffectsListSocket));
	if (!BorderPtr)
	{
		return nullptr;
	}

	for (auto Iter : BorderPtr->GetAllChildren())
	{
		auto UIPtr = Cast<UEffectsList>(Iter);
		if (UIPtr)
		{
			// 绑定效果状态栏
			auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
			if (CharacterPtr)
			{
				UIPtr->BindCharacterState(CharacterPtr);
			}

			return UIPtr;
		}
	}
	return nullptr;
}

void URegularActionLayout::DisplayTeamInfo(
	bool bIsDisplay,
	AHumanCharacter* HumanCharacterPtr
)
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(HUD_TeamSocket));
	if (!BorderPtr)
	{
		return;
	}

	auto UIPtr = Cast<UHUD_TeamInfo>(BorderPtr->GetContent());
	if (UIPtr)
	{
		if (bIsDisplay)
		{
			UIPtr->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UIPtr->RemoveFromParent();
		}
	}
	else
	{
		if (bIsDisplay)
		{
			UIPtr = CreateWidget<UHUD_TeamInfo>(GetWorldImp(), HUD_TeamInfoClass);
			if (UIPtr)
			{
				BorderPtr->AddChild(UIPtr);
			}
		}
	}
}

UProgressTips* URegularActionLayout::ViewProgressTips(
	bool bIsViewMenus
)
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(ProgressTipsSocket));
	if (!BorderPtr)
	{
		return nullptr;
	}

	if (bIsViewMenus)
	{
		if (BorderPtr->HasAnyChildren())
		{
			return nullptr;
		}

		auto UIPtr = CreateWidget<UProgressTips>(GetWorldImp(), ProgressTipsClass);
		if (UIPtr)
		{
			BorderPtr->AddChild(UIPtr);

			return UIPtr;
		}
	}

	BorderPtr->ClearChildren();

	return nullptr;
}

void URegularActionLayout::OnHPChanged(
	const FOnAttributeChangeData&


)
{
	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacterPtr)
	{
		return;
	}

	auto CharacterAttributesRef = PlayerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

	const auto CurrentValue =
		CharacterAttributesRef->GetHP();
	const auto MaxValue =
		CharacterAttributesRef->GetMax_HP();
	const auto LowerHP_Percent =
		UGameOptions::GetInstance()->LowerHP_Percent;

	const auto bIsLowerHP = CurrentValue < (MaxValue * (LowerHP_Percent / 100.f));

	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FRegularActionLayout::Get().LowerHPSocket));
	if (!BorderPtr)
	{
		return;
	}

	BorderPtr->SetVisibility(bIsLowerHP ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
