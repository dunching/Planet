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
#include "Tools.h"
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

UMainHUDLayout* UUIManagerSubSystem::GetMainHUDLayout() const
{
	return UGameplayStatics::GetPlayerController(this, 0)->GetHUD<AMainHUD>()->GetMainHUDLayout();
}

void UUIManagerSubSystem::SwitchLayout(
	ELayoutCommon MainHUDType,
	const ILayoutInterfacetion::FOnQuit& OnQuit
	)
{
	GetMainHUDLayout()->SwitchToNewLayout(MainHUDType, OnQuit);
}

FString UUIManagerSubSystem::GetLayoutName(
	ELayoutCommon MainHUDType
	) const
{
	FString Result = TEXT("");
	switch (MainHUDType)
	{
	case ELayoutCommon::kEmptyLayout:
		break;
	case ELayoutCommon::kActionLayout:
		break;
	case ELayoutCommon::kMenuLayout:
		break;
	case ELayoutCommon::kEndangeredLayout:
		break;
	case ELayoutCommon::kOptionLayout:
		break;
	case ELayoutCommon::kConversationLayout:
		break;
	case ELayoutCommon::kTransactionLayout:
		break;
	case ELayoutCommon::kBuildingLayout:
		break;
	case ELayoutCommon::kViewTasksLayout:
		break;
	case ELayoutCommon::kTransitionLayout:
		break;
	case ELayoutCommon::kObserverLayout:
		break;
	default: ;
	}
	return Result;
}

void UUIManagerSubSystem::SwitchMenuLayout(
	EMenuType MenuType
	)
{
	auto MenuLayoutPtr = GetMainHUDLayout()->GetMenuLayout();

	if (MenuLayoutPtr)
	{
		MenuLayoutPtr->SwitchViewer(MenuType);
	}
}

FString UUIManagerSubSystem::GetMenuLayoutName(
	EMenuType MenuType
	) const
{
	FString Result = TEXT("");
	switch (MenuType)
	{
	case EMenuType::kEmpty:
		break;
	case EMenuType::kAllocationSkill:
		{
			Result = TEXT("AllocationSkills");
		}
		break;
	case EMenuType::kAllocationTalent:
		break;
	case EMenuType::kGroupManagger:
		{
			Result = TEXT("GroupManagger");
		}
		break;
	case EMenuType::kRaffle:
		break;
	case EMenuType::kViewTask:
		break;
	case EMenuType::kViewMap:
		break;
	}
	return Result;
}

UMainMenuLayout* UUIManagerSubSystem::GetMainMenuLayout() const
{
	return GetMainHUDLayout()->GetMenuLayout();
}
