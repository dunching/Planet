
#include "HUD_TeamInfo.h"

#include <Kismet/GameplayStatics.h>
#include <Components/VerticalBox.h>
#include <Components/Border.h>

#include "PlanetControllerInterface.h"
#include "GroupMnaggerComponent.h"
#include "TeamMateInfo.h"
#include "HUD_TeamMateInfo.h"
#include "CharacterBase.h"

namespace GroupManaggerMenu
{
	const FName VerticalBox = TEXT("VerticalBox");

	const FName FollowOpetion = TEXT("FollowOpetion");

	const FName AssistanceOption = TEXT("AssistanceOption");
}

void UHUD_TeamInfo::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UHUD_TeamInfo::NativeDestruct()
{
	if (TeammateOptionChangedDelegateContainer)
	{
		TeammateOptionChangedDelegateContainer->UnBindCallback();
	}

	Super::NativeDestruct();
}

FReply UHUD_TeamInfo::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::F1)
	{

	}
	else if (InKeyEvent.GetKey() == EKeys::F2)
	{

	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UHUD_TeamInfo::ResetUIByData()
{
	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(GroupManaggerMenu::VerticalBox));
	if (!PanelPtr)
	{
		return;
	}
	PanelPtr->ClearChildren();

	auto PCPtr = Cast<IPlanetControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}

	auto GMCPtr = PCPtr->GetGroupMnaggerComponent();

	auto MembersHelperSPtr = GMCPtr->GetTeamHelper();
	if (MembersHelperSPtr)
	{
		for (auto Iter : MembersHelperSPtr->MembersSet)
		{
			auto WidgetPtr = CreateWidget<UHUD_TeamMateInfo>(this, TeamMateInfoClass);
			if (WidgetPtr)
			{
				PanelPtr->AddChild(WidgetPtr);

				WidgetPtr->ResetToolUIByData(Iter);
			}
		}
	}

	TeammateOptionChangedDelegateContainer = MembersHelperSPtr->TeammateOptionChanged.AddCallback(
		std::bind(&ThisClass::OnTeammateOptionChanged, this, std::placeholders::_1, std::placeholders::_2
		));

	OnTeammateOptionChanged(GMCPtr->GetTeamHelper()->GetTeammateOption(), GMCPtr->GetTeamHelper()->OwnerCharacterUnitPtr);
}

void UHUD_TeamInfo::OnTeammateOptionChanged(
	ETeammateOption TeammateOption,
	const TSharedPtr<FCharacterUnitType>& LeaderPCPtr
)
{
	{
		auto BorderPtr = Cast<UBorder>(GetWidgetFromName(GroupManaggerMenu::FollowOpetion));
		if (BorderPtr)
		{
			switch (TeammateOption)
			{
			case ETeammateOption::kFollow:
			{
				BorderPtr->SetVisibility(ESlateVisibility::Visible);
			}
			break;
			case ETeammateOption::kAssistance:
			{
				BorderPtr->SetVisibility(ESlateVisibility::Hidden);
			}
			break;
			}
		}
	}
	{
		auto BorderPtr = Cast<UBorder>(GetWidgetFromName(GroupManaggerMenu::AssistanceOption));
		if (BorderPtr)
		{
			switch (TeammateOption)
			{
			case ETeammateOption::kFollow:
			{
				BorderPtr->SetVisibility(ESlateVisibility::Hidden);
			}
			break;
			case ETeammateOption::kAssistance:
			{
				BorderPtr->SetVisibility(ESlateVisibility::Visible);
			}
			break;
			}
		}
	}
}
