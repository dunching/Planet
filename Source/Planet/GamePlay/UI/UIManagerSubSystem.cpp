
#include "UIManagerSubSystem.h"

#include "Async/Async.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Overlay.h"

#include "GameMode/PlanetGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "InteractionToAIMenu.h"
#include "CreateMenu.h"
#include "PromptBox.h"
#include "backpackMenu.h"
#include "ToolsMenu.h"
#include "CharacterInteraction/Piginteraction.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "DestroyProgress.h"
#include "MainUILayout.h"
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
#include "GetItemInfos.h"
#include "RaffleMenu.h"
#include "PlanetPlayerState.h"
#include "UICommon.h"

namespace UIManagerSubSystem
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");
}

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
	MainUILayoutPtr = GetMainUILAyout();
	if (!MainUILayoutPtr)
	{
		return;
	}
	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(MainUILayoutPtr->PawnActionStateHUDSocket));
	if (!BorderPtr)
	{
		return;
	}

	auto UIPtr = Cast<UPawnStateActionHUD>(BorderPtr->GetContent());
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
			UIPtr = CreateWidget<UPawnStateActionHUD>(GetWorldImp(), MainUILayoutPtr->PawnStateActionHUDClass);
			if (UIPtr)
			{
				UIPtr->CharacterPtr = CharacterPtr;
				BorderPtr->AddChild(UIPtr);
			}
		}
	}
}

void UUIManagerSubSystem::DisplayBuildingStateHUD(bool bIsDisplay)
{
	MainUILayoutPtr = GetMainUILAyout();
	if (!MainUILayoutPtr)
	{
		return;
	}
	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(MainUILayoutPtr->PawnBuildingStateHUDSocket));
	if (!BorderPtr)
	{
		return;
	}

	auto UIPtr = Cast<UPawnStateBuildingHUD>(BorderPtr->GetContent());
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
			UIPtr = CreateWidget<UPawnStateBuildingHUD>(GetWorldImp(), MainUILayoutPtr->PawnStateBuildingHUDClass);
			if (UIPtr)
			{
				BorderPtr->AddChild(UIPtr);
			}
		}
	}
}

void UUIManagerSubSystem::ViewBackpack(bool bIsDisplay, const FSceneToolsContainer& NewSPHoldItemPerperty)
{
	MainUILayoutPtr = GetMainUILAyout();
	if (!MainUILayoutPtr)
	{
		return;
	}
	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(MainUILayoutPtr->RightBackpackSocket));
	if (!BorderPtr)
	{
		return;
	}

	auto UIPtr = Cast<UBackpackMenu>(BorderPtr->GetContent());
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
			UIPtr = CreateWidget<UBackpackMenu>(GetWorldImp(), MainUILayoutPtr->BackpackMenuClass);
			if (UIPtr)
			{
				UIPtr->SetHoldItemProperty(NewSPHoldItemPerperty);

				BorderPtr->AddChild(UIPtr);
			}
		}
	}
}

void UUIManagerSubSystem::ViewSkills(bool bIsDisplay, const FSceneToolsContainer& NewSPHoldItemPerperty)
{
	MainUILayoutPtr = GetMainUILAyout();
	if (!MainUILayoutPtr)
	{
		return;
	}

	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(MainUILayoutPtr->AllocationSkillsSocket));
	if (!BorderPtr)
	{
		return;
	}

	auto UIPtr = Cast<UAllocationSkillsMenu>(BorderPtr->GetContent());
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
			UIPtr = CreateWidget<UAllocationSkillsMenu>(GetWorldImp(), MainUILayoutPtr->AllocationSkillsMenuClass);
			if (UIPtr)
			{
				UIPtr->SetHoldItemProperty(NewSPHoldItemPerperty);

				BorderPtr->AddChild(UIPtr);
			}
		}
	}
}

void UUIManagerSubSystem::ViewTalentAllocation(bool bIsDisplay)
{
	MainUILayoutPtr = GetMainUILAyout();
	if (!MainUILayoutPtr)
	{
		return;
	}

	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(MainUILayoutPtr->TalentAllocationSocket));
	if (!BorderPtr)
	{
		return;
	}

	auto UIPtr = Cast<UTalentAllocation>(BorderPtr->GetContent());
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
			UIPtr = CreateWidget<UTalentAllocation>(GetWorldImp(), MainUILayoutPtr->TalentAllocationClass);
			if (UIPtr)
			{
				BorderPtr->AddChild(UIPtr);
			}
		}
	}
}

void UUIManagerSubSystem::ViewGroupMatesManagger(bool bIsDisplay, AHumanCharacter* HumanCharacterPtr)
{
	MainUILayoutPtr = GetMainUILAyout();
	if (!MainUILayoutPtr)
	{
		return;
	}

	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(MainUILayoutPtr->GroupMatesManaggerSocket));
	if (!BorderPtr)
	{
		return;
	}

	auto UIPtr = Cast<UGroupManaggerMenu>(BorderPtr->GetContent());
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
			UIPtr = CreateWidget<UGroupManaggerMenu>(GetWorldImp(), MainUILayoutPtr->GroupManaggerMenuClass);
			if (UIPtr)
			{
				UIPtr->HumanCharacterPtr = HumanCharacterPtr;
				BorderPtr->AddChild(UIPtr);
			}
		}
	}
}

void UUIManagerSubSystem::DisplayTeamInfo(bool bIsDisplay, AHumanCharacter* HumanCharacterPtr)
{
	MainUILayoutPtr = GetMainUILAyout();
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
	if (RaffleMenuPtr)
	{
		if (bIsDisplay)
		{
			RaffleMenuPtr->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			RaffleMenuPtr->RemoveFromParent();
			RaffleMenuPtr = nullptr;
		}
	}
	else
	{
		if (bIsDisplay)
		{
			RaffleMenuPtr = CreateWidget<URaffleMenu>(GetWorldImp(), UAssetRefMap::GetInstance()->RaffleMenuClass);
			if (RaffleMenuPtr)
			{
				auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
				if (CharacterPtr)
				{
					RaffleMenuPtr->SetHoldItemProperty(
						CharacterPtr->GetPlayerState<APlanetPlayerState>()->GetHoldingItemsComponent()->GetHoldItemProperty()
					);
				}
				RaffleMenuPtr->AddToViewport(EUIOrder::kRaffle);
			}
		}
	}
}

UEffectsList* UUIManagerSubSystem::ViewEffectsList(bool bIsViewMenus)
{
	MainUILayoutPtr = GetMainUILAyout();
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

UGetItemInfos* UUIManagerSubSystem::GetItemInfos()
{
	auto BorderPtr = Cast<UBorder>(MainUILayoutPtr->GetWidgetFromName(UIManagerSubSystem::GetItemInfos_Socket));
	if (!BorderPtr)
	{
		return nullptr;
	}

	for (auto Iter : BorderPtr->GetAllChildren())
	{
		auto UIPtr = Cast<UGetItemInfos>(Iter);
		if (UIPtr)
		{
			return UIPtr;
		}
	}

	return nullptr;
}

UMainUILayout* UUIManagerSubSystem::GetMainUILAyout()
{
	if (!MainUILayoutPtr)
	{
		MainUILayoutPtr = CreateWidget<UMainUILayout>(GetWorldImp(), UAssetRefMap::GetInstance()->MainUILayoutClass);
		if (MainUILayoutPtr)
		{
			MainUILayoutPtr->AddToViewport(EUIOrder::kMainUI);

			DisplayActionStateHUD(false);
			DisplayBuildingStateHUD(false);
			ViewBackpack(false);
			ViewSkills(false);
			ViewTalentAllocation(false);
			ViewGroupMatesManagger(false);
			DisplayTeamInfo(false);
			ViewEffectsList(false);
			ViewProgressTips(false);
			ViewRaffleMenu(false);
		}
	}

	return MainUILayoutPtr;
}