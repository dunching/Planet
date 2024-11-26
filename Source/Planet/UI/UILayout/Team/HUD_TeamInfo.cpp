
#include "HUD_TeamInfo.h"

#include <Kismet/GameplayStatics.h>
#include <Components/VerticalBox.h>
#include <Components/Border.h>

#include "PlanetControllerInterface.h"
#include "GroupMnaggerComponent.h"
#include "TeamMateInfo.h"
#include "HUD_TeamMateInfo.h"
#include "CharacterBase.h"
#include "TemplateHelper.h"
#include "GroupSharedInfo.h"
#include "ItemProxy_Character.h"

struct FHUD_TeamInfo : public TStructVariable<FHUD_TeamInfo>
{
	const FName VerticalBox = TEXT("VerticalBox");

	const FName FollowOpetion = TEXT("FollowOpetion");

	const FName AssistanceOption = TEXT("AssistanceOption");
};

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
	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(FHUD_TeamInfo::Get().VerticalBox));
	if (!PanelPtr)
	{
		return;
	}
	auto PCPtr = Cast<IPlanetControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}

	auto ChidrensAry = PanelPtr->GetAllChildren();
	int32 Index = 0;

	auto GMCPtr = PCPtr->GetGroupSharedInfo();

	auto MembersHelperSPtr = GMCPtr->GetTeamMatesHelperComponent();
	if (MembersHelperSPtr)
	{
		for (auto Iter : MembersHelperSPtr->MembersSet)
		{
			if (Index < ChidrensAry.Num())
			{
				ChidrensAry[Index]->SetVisibility(ESlateVisibility::Visible);
				auto WidgetPtr = Cast<UHUD_TeamMateInfo>(ChidrensAry[Index]);
				if (WidgetPtr)
				{
					WidgetPtr->ResetToolUIByData(Iter);
				}
			}
			Index++;
		}
	}

	for (; Index < ChidrensAry.Num(); Index++)
	{
		ChidrensAry[Index]->SetVisibility(ESlateVisibility::Hidden);
	}

	TeammateOptionChangedDelegateContainer = MembersHelperSPtr->TeammateOptionChanged.AddCallback(
		std::bind(&ThisClass::OnTeammateOptionChanged, this, std::placeholders::_1, std::placeholders::_2
		));

	OnTeammateOptionChanged(GMCPtr->GetTeamMatesHelperComponent()->GetTeammateOption(), GMCPtr->GetTeamMatesHelperComponent()->OwnerCharacterUnitPtr);
}

void UHUD_TeamInfo::OnTeammateOptionChanged(
	ETeammateOption TeammateOption,
	const TSharedPtr<FCharacterUnitType>& LeaderPCPtr
)
{
	{
		auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().FollowOpetion));
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
		auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().AssistanceOption));
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
