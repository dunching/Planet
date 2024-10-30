
#include "UIManagerSubSystem.h"

#include "Async/Async.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Overlay.h"

#include "GameMode_Main.h"
#include "Kismet/GameplayStatics.h"
#include "CreateMenu.h"
#include "PromptBox.h"
#include "backpackMenu.h"
#include "ToolsMenu.h"
#include "CharacterInteraction/Piginteraction.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "DestroyProgress.h"
#include "RegularActionLayout.h"
#include "PawnStateActionHUD.h"
#include "PawnStateBuildingHUD.h"
#include "BackpackIcon.h"
#include "HoldingItemsComponent.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "AllocationSkillsMenu.h"
#include "CharacterAttibutes.h"
#include "EffectsList.h"
#include "ProgressTips.h"
#include "TalentAllocation.h"
#include "GroupManaggerMenu.h"
#include "HUD_TeamInfo.h"
#include "GetItemInfosList.h"
#include "RaffleMenu.h"
#include "MainHUD.h"
#include "UICommon.h"
#include "MenuLayout.h"
#include "PawnStateConsumablesHUD.h"
#include "FocusTitle.h"
#include "PlanetPlayerController.h"
#include "FocusIcon.h"

struct FUIManagerSubSystem : public TStructVariable<FUIManagerSubSystem>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName FocusCharacterSocket = TEXT("FocusCharacterSocket");
};

UUIManagerSubSystem* UUIManagerSubSystem::GetInstance()
{
	return Cast<UUIManagerSubSystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), UUIManagerSubSystem::StaticClass())
	);
}

UUIManagerSubSystem::UUIManagerSubSystem() :
	Super()
{

}

UUIManagerSubSystem::~UUIManagerSubSystem()
{
}

void UUIManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UUIManagerSubSystem::DisplayActionStateHUD(bool bIsDisplay, ACharacterBase* CharacterPtr)
{
	auto MainHUDPtr = Cast<AMainHUD>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0)->GetHUD());
	if (!MainHUDPtr)
	{
		return;
	}
	MainHUDPtr->SwitchState(bIsDisplay ? EMainHUDType::kRegularAction : EMainHUDType::kNone);
}

void UUIManagerSubSystem::DisplayEndangeredState(bool bIsDisplay)
{
	auto MainHUDPtr = Cast<AMainHUD>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0)->GetHUD());
	if (MainHUDPtr)
	{
		MainHUDPtr->SwitchState(bIsDisplay ? EMainHUDType::kEndangered : EMainHUDType::kNone);
	}
}

void UUIManagerSubSystem::DisplayBuildingStateHUD(bool bIsDisplay)
{
}

void UUIManagerSubSystem::SwitchMenu(bool bIsShow)
{
	MenuLayoutPtr = GetMainMenu();
	if (!MenuLayoutPtr)
	{
		return;
	}

	MenuLayoutPtr->SetVisibility(bIsShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	if (bIsShow)
	{
	}
	else
	{
		MenuLayoutPtr->SyncData();
	}
}

void UUIManagerSubSystem::ViewBackpack(bool bIsDisplay)
{
	SwitchMenu(bIsDisplay);
	if (bIsDisplay)
	{
		MenuLayoutPtr = GetMainMenu();
		MenuLayoutPtr->SwitchViewer(EMenuType::kAllocationSkill);
	}
	else
	{
	}
}

void UUIManagerSubSystem::ViewTalentAllocation(bool bIsDisplay)
{
	SwitchMenu(bIsDisplay);
	if (bIsDisplay)
	{
		MenuLayoutPtr = GetMainMenu();
		MenuLayoutPtr->SwitchViewer(EMenuType::kAllocationTalent);
	}
	else
	{
	}
}

void UUIManagerSubSystem::ViewGroupMatesManagger(bool bIsDisplay, AHumanCharacter* HumanCharacterPtr)
{
}

void UUIManagerSubSystem::DisplayTeamInfo(bool bIsDisplay, AHumanCharacter* HumanCharacterPtr)
{
	auto MainUILayoutPtr = GetRegularActionState();
	if (!MainUILayoutPtr)
	{
		return;
	}

	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(MainUILayoutPtr->HUD_TeamSocket));
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
			UIPtr = CreateWidget<UHUD_TeamInfo>(GetWorldImp(), MainUILayoutPtr->HUD_TeamInfoClass);
			if (UIPtr)
			{
				BorderPtr->AddChild(UIPtr);
			}
		}
	}
}

void UUIManagerSubSystem::ViewRaffleMenu(bool bIsDisplay)
{
}

UEffectsList* UUIManagerSubSystem::ViewEffectsList(bool bIsViewMenus)
{
	auto MainUILayoutPtr = GetRegularActionState();
	if (!MainUILayoutPtr)
	{
		return nullptr;
	}

	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(MainUILayoutPtr->EffectsListSocket));
	if (!BorderPtr)
	{
		return nullptr;
	}

	if (bIsViewMenus)
	{
		for (auto Iter : BorderPtr->GetAllChildren())
		{
			auto UIPtr = Cast<UEffectsList>(Iter);
			if (UIPtr)
			{
				return UIPtr;
			}
		}
		auto UIPtr = CreateWidget<UEffectsList>(GetWorldImp(), MainUILayoutPtr->EffectsListClass);
		if (UIPtr)
		{
			UIPtr->bIsPositiveSequence = false;
			BorderPtr->AddChild(UIPtr);

			return UIPtr;
		}
	}
	else
	{
		BorderPtr->ClearChildren();

		return nullptr;
	}

	return nullptr;
}

UProgressTips* UUIManagerSubSystem::ViewProgressTips(bool bIsViewMenus)
{
	auto MainUILayoutPtr = GetRegularActionState();
	if (!MainUILayoutPtr)
	{
		return nullptr;
	}

	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(MainUILayoutPtr->ProgressTipsSocket));
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

		auto UIPtr = CreateWidget<UProgressTips>(GetWorldImp(), MainUILayoutPtr->ProgressTipsClass);
		if (UIPtr)
		{
			BorderPtr->AddChild(UIPtr);

			return UIPtr;
		}
	}

	BorderPtr->ClearChildren();

	return nullptr;
}

void UUIManagerSubSystem::OnFocusCharacter(ACharacterBase* TargetCharacterPtr)
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

	auto MainUILayoutPtr = GetRegularActionState();
	if (!MainUILayoutPtr)
	{
		return;
	}

	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(FUIManagerSubSystem::Get().FocusCharacterSocket));
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
			UIPtr = CreateWidget<UFocusTitle>(GetWorldImp(), MainUILayoutPtr->FocusTitleClass);
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

void UUIManagerSubSystem::InitialUI()
{
	DisplayActionStateHUD(false);
	DisplayBuildingStateHUD(false);
	DisplayTeamInfo(false);
	ViewEffectsList(false);
	ViewProgressTips(false);
	OnFocusCharacter(nullptr);

	InitialHoverUI();
}

void UUIManagerSubSystem::InitialHoverUI()
{
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PCPtr)
	{
		auto DelegateHandle = 
			PCPtr->OnFocusCharacterDelegate.AddCallback(std::bind(&ThisClass::OnFocusCharacter, this, std::placeholders::_1));
		DelegateHandle->bIsAutoUnregister = false;
	}
}

URegularActionLayout* UUIManagerSubSystem::GetRegularActionState()
{
	auto MainHUDPtr = Cast<AMainHUD>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0)->GetHUD());
	if (MainHUDPtr)
	{
		return MainHUDPtr->RegularActionStatePtr;
	}

	return nullptr;
}

UMenuLayout* UUIManagerSubSystem::GetMainMenu()
{
	if (!MenuLayoutPtr)
	{
		MenuLayoutPtr = CreateWidget<UMenuLayout>(GetWorldImp(), UAssetRefMap::GetInstance()->MenuLayoutClass);
		if (MenuLayoutPtr)
		{
			MenuLayoutPtr->AddToViewport(EUIOrder::kTableMenu);

			ViewBackpack(false);
			ViewTalentAllocation(false);
			ViewGroupMatesManagger(false);
			ViewRaffleMenu(false);
		}
	}

	return MenuLayoutPtr;
}
