#include "HUD_TeamInfo.h"

#include <Kismet/GameplayStatics.h>
#include <Components/VerticalBox.h>
#include <Components/Border.h>

#include "PlanetControllerInterface.h"
#include "TeamMatesHelperComponent.h"
#include "TeamMateInfo.h"
#include "HUD_TeamMateInfo.h"
#include "CharacterBase.h"
#include "TemplateHelper.h"
#include "GroupManagger.h"
#include "ItemProxy_Character.h"

struct FHUD_TeamInfo : public TStructVariable<FHUD_TeamInfo>
{
	const FName VerticalBox = TEXT("VerticalBox");

	const FName FollowOpetion = TEXT("FollowOpetion");

	const FName AssistanceOption = TEXT("AssistanceOption");

	const FName FireTarget = TEXT("FireTarget");
};

void UHUD_TeamInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHUD_TeamInfo::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UHUD_TeamInfo::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent
)
{
	if (InKeyEvent.GetKey() == EKeys::F1)
	{
	}
	else if (InKeyEvent.GetKey() == EKeys::F2)
	{
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UHUD_TeamInfo::Enable()
{
	ILayoutItemInterfacetion::Enable();
	
	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(FHUD_TeamInfo::Get().VerticalBox));
	if (!PanelPtr)
	{
		return;
	}
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto ChidrensAry = PanelPtr->GetAllChildren();
	int32 Index = 0;

	auto GMCPtr = CharacterPtr->GetGroupManagger();

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
		std::bind(
			&ThisClass::OnTeammateOptionChanged,
			this,
			std::placeholders::_1,
			std::placeholders::_2
		)
	);

	OnTeammateOptionChanged(
		GMCPtr->GetTeamMatesHelperComponent()->GetTeammateOption(),
		GMCPtr->GetTeamMatesHelperComponent()->GetOwnerCharacterProxy()
	);
}

void UHUD_TeamInfo::DisEnable()
{
	if (TeammateOptionChangedDelegateContainer)
	{
		TeammateOptionChangedDelegateContainer->UnBindCallback();
	}

	ILayoutItemInterfacetion::DisEnable();
}

void UHUD_TeamInfo::OnTeammateOptionChanged(
	ETeammateOption TeammateOption,
	const TSharedPtr<FCharacterProxyType>& LeaderPCPtr
)
{
	switch (TeammateOption)
	{
	case ETeammateOption::kFollow:
		{
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().FollowOpetion));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Visible);
				}
			}
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().AssistanceOption));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Hidden);
				}
			}
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().FireTarget));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
		break;
	case ETeammateOption::kAssistance:
		{
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().FollowOpetion));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Hidden);
				}
			}
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().AssistanceOption));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Visible);
				}
			}
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().FireTarget));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
		break;
	case ETeammateOption::kFireTarget:
		{
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().FollowOpetion));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Hidden);
				}
			}
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().AssistanceOption));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Hidden);
				}
			}
			{
				auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FHUD_TeamInfo::Get().FireTarget));
				if (BorderPtr)
				{
					BorderPtr->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
		break;
	}
}
