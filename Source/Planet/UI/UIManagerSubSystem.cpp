#include "UIManagerSubSystem.h"

#include "Async/Async.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Overlay.h"

#include "GameMode_Main.h"
#include "Kismet/GameplayStatics.h"
#include "CreateMenu.h"
#include "PromptBox.h"
#include "DestroyProgress.h"
#include "MainHUD.h"
#include "UICommon.h"
#include "MainMenuLayout.h"
#include "PawnStateConsumablesHUD.h"
#include "FocusTitle.h"
#include "PlanetPlayerController.h"
#include "FocusIcon.h"
#include "MainHUDLayout.h"

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
	                                 USubsystemBlueprintLibrary::GetGameInstanceSubsystem(
		                                  GetWorldImp(),
		                                  UUIManagerSubSystem::StaticClass()
		                                 )
	                                );
}

UUIManagerSubSystem::UUIManagerSubSystem() :
                                           Super()
{
}

UUIManagerSubSystem::~UUIManagerSubSystem()
{
}

void UUIManagerSubSystem::Initialize(
	FSubsystemCollectionBase& Collection
	)
{
	Super::Initialize(Collection);
}

void UUIManagerSubSystem::SwitchLayout(
	ELayoutCommon MainHUDType,
	const ILayoutInterfacetion::FOnQuit& OnQuit
	)
{
	UGameplayStatics::GetPlayerController(this, 0)->GetHUD<AMainHUD>()->SwitchLayout(MainHUDType, OnQuit);
}

void UUIManagerSubSystem::SwitchMenuLayout(
	EMenuType MenuType
	)
{
	auto MenuLayoutPtr = UGameplayStatics::GetPlayerController(this, 0)
	                     ->GetHUD<AMainHUD>()
	                     ->GetMainHUDLayout()
	                     ->GetMenuLayout();

	if (MenuLayoutPtr)
	{
		MenuLayoutPtr->SwitchViewer(MenuType);
	}
}
