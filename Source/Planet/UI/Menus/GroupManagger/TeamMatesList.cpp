
#include "TeamMatesList.h"

#include <Kismet/GameplayStatics.h>
#include "Components/VerticalBox.h"

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "HoldingItemsComponent.h"
#include "GenerateType.h"
#include "GroupMnaggerComponent.h"
#include "CharacterBase.h"
#include "GroupMateInfo.h"
#include "PlanetControllerInterface.h"
#include "ItemProxy.h"
#include "HumanCharacter.h"

#include "TeamMateInfo.h"

namespace TeanMatesList
{
	const FName VerticalBox = TEXT("VerticalBox");
}

void UTeamMatesList::NativeConstruct()
{
	Super::NativeConstruct();

	HumanCharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	ResetUIByData();
}

void UTeamMatesList::NativeDestruct()
{
	DelegateAry.Empty();

	Super::NativeDestruct();
}

void UTeamMatesList::ResetUIByData()
{
// 	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(TeanMatesList::VerticalBox));
// 	if (!PanelPtr)
// 	{
// 		return;
// 	}
// 	PanelPtr->ClearChildren();
// 	DelegateAry.Empty();
// 
// 	if (!HumanCharacterPtr)
// 	{
// 		return;
// 	}
// 
// 	auto GMCPtr = HumanCharacterPtr->GetGroupSharedInfo();
// 
// 	int32 CurrentMemberNum = 0;
// 	auto MembersHelperSPtr = GMCPtr->GetTeamHelper();
// 	if (MembersHelperSPtr)
// 	{
// 		for (auto Iter : MembersHelperSPtr->MembersSet)
// 		{
// 			CurrentMemberNum++;
// 			auto WidgetPtr = CreateWidget<UTeamMateInfo>(this, TeamMateInfoClass);
// 			if (WidgetPtr)
// 			{
// 				PanelPtr->AddChild(WidgetPtr);
// 
// 				WidgetPtr->ResetToolUIByData(Iter);
// 				DelegateAry.Add(WidgetPtr->OnDroped.AddCallback(std::bind(&ThisClass::OnTeammateChanged, this, std::placeholders::_1)));
// 			}
// 		}
// 	}
// 	for (; CurrentMemberNum < MaxMemberNum; CurrentMemberNum++)
// 	{
// 		auto WidgetPtr = CreateWidget<UTeamMateInfo>(this, TeamMateInfoClass);
// 		if (WidgetPtr)
// 		{
// 			PanelPtr->AddChild(WidgetPtr);
// 
// 			WidgetPtr->ResetToolUIByData(nullptr);
// 			DelegateAry.Add(WidgetPtr->OnDroped.AddCallback(std::bind(&ThisClass::OnTeammateChanged, this, std::placeholders::_1)));
// 		}
// 	}
}

void UTeamMatesList::SyncData()
{

}

void UTeamMatesList::OnTeammateChanged(UTeamMateInfo* GourpMateUnitPtr)
{
	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(TeanMatesList::VerticalBox));
	if (!PanelPtr)
	{
		return;
	}

	auto Childrens = PanelPtr->GetAllChildren();
	for (auto Iter : Childrens)
	{
		auto TeamMateInfoPtr = Cast<UTeamMateInfo>(Iter);
		if (TeamMateInfoPtr)
		{
			if (TeamMateInfoPtr == GourpMateUnitPtr)
			{
				continue;
			}
			else
			{
				if (TeamMateInfoPtr->GroupMateUnitPtr == GourpMateUnitPtr->GroupMateUnitPtr)
				{
					TeamMateInfoPtr->ResetToolUIByData(nullptr);
				}
			}

		}
	}
}
