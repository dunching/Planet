
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
	SwitchMenu(bIsDisplay);
	if (bIsDisplay)
	{
		MenuLayoutPtr = GetMainMenu();
		MenuLayoutPtr->SwitchViewer(EMenuType::kGroupManagger);
	}
	else
	{
	}
}

void UUIManagerSubSystem::ViewRaffleMenu(bool bIsDisplay)
{
}

void UUIManagerSubSystem::InitialUI()
{
	DisplayActionLayout(false);
	DisplayBuildingLayout(false);
}

void UUIManagerSubSystem::DisplayActionLayout(bool bIsDisplay, ACharacterBase* CharacterPtr)
{
	auto MainHUDPtr = Cast<AMainHUD>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0)->GetHUD());
	if (!MainHUDPtr)
	{
		return;
	}
	MainHUDPtr->SwitchState(bIsDisplay ? EMainHUDType::kRegularAction : EMainHUDType::kNone);
}

void UUIManagerSubSystem::DisplayEndangeredLayout(bool bIsDisplay)
{
	auto MainHUDPtr = Cast<AMainHUD>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0)->GetHUD());
	if (MainHUDPtr)
	{
		MainHUDPtr->SwitchState(bIsDisplay ? EMainHUDType::kEndangered : EMainHUDType::kNone);
	}
}

void UUIManagerSubSystem::DisplayBuildingLayout(bool bIsDisplay)
{
}

void UUIManagerSubSystem::CloseLayout()
{
	auto MainHUDPtr = Cast<AMainHUD>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0)->GetHUD());
	if (MainHUDPtr)
	{
		MainHUDPtr->SwitchState(EMainHUDType::kNone);
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
		}
	}

	return MenuLayoutPtr;
}
