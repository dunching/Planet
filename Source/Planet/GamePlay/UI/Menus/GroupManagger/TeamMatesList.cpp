
#include "TeamMatesList.h"

#include <Kismet/GameplayStatics.h>
#include "Components/VerticalBox.h"

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"
#include "HoldingItemsComponent.h"
#include "GenerateType.h"
#include "GroupMnaggerComponent.h"
#include "CharacterBase.h"
#include "GroupMateInfo.h"
#include "HumanControllerInterface.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "GroupsManaggerSubSystem.h"
#include "TeamMateInfo.h"

namespace TeanMatesList
{
	const FName VerticalBox = TEXT("VerticalBox");
}

void UTeamMatesList::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UTeamMatesList::NativeDestruct()
{
	DelegateAry.Empty();

	Super::NativeDestruct();
}

void UTeamMatesList::ResetUIByData()
{
	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(TeanMatesList::VerticalBox));
	if (!PanelPtr)
	{
		return;
	}
	PanelPtr->ClearChildren();
	DelegateAry.Empty();

	if (!HumanCharacterPtr)
	{
		return;
	}

	auto GMCPtr = HumanCharacterPtr->GetGroupMnaggerComponent();

	int32 CurrentMemberNum = 0;
	auto MembersHelperSPtr = GMCPtr->GetTeamsHelper();
	if (MembersHelperSPtr)
	{
		for (auto Iter : MembersHelperSPtr->MembersMap)
		{
			CurrentMemberNum++;
			auto WidgetPtr = CreateWidget<UTeamMateInfo>(this, TeamMateInfoClass);
			if (WidgetPtr)
			{
				PanelPtr->AddChild(WidgetPtr);

				WidgetPtr->ResetToolUIByData(Iter.Key);
				DelegateAry.Add(WidgetPtr->OnDroped.AddCallback(std::bind(&ThisClass::OnTeammateChanged, this, std::placeholders::_1)));
			}
		}
	}
	for (; CurrentMemberNum < MaxMemberNum; CurrentMemberNum++)
	{
		auto WidgetPtr = CreateWidget<UTeamMateInfo>(this, TeamMateInfoClass);
		if (WidgetPtr)
		{
			PanelPtr->AddChild(WidgetPtr);

			WidgetPtr->ResetToolUIByData(nullptr);
			DelegateAry.Add(WidgetPtr->OnDroped.AddCallback(std::bind(&ThisClass::OnTeammateChanged, this, std::placeholders::_1)));
		}
	}
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