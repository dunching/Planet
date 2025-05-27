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
#include "InventoryComponent.h"
#include "GenerateTypes.h"
#include "TeamMatesHelperComponent.h"
#include "CharacterBase.h"
#include "GroupMateInfo.h"
#include "PlanetControllerInterface.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter.h"
#include "ScopeValue.h"

#include "TeamMateInfo.h"

struct TeanMatesList : public TStructVariable<TeanMatesList>
{
	const FName VerticalBox = TEXT("VerticalBox");
};

void UTeamMatesList::NativeConstruct()
{
	Super::NativeConstruct();

	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(TeanMatesList::Get().VerticalBox));
	if (!PanelPtr)
	{
		return;
	}
	auto ChildrensAry = PanelPtr->GetAllChildren();
	for (auto Iter : ChildrensAry)
	{
		auto WidgetPtr = Cast<UTeamMateInfo>(Iter);
		if (WidgetPtr)
		{
			DelegateAry.Add(
				WidgetPtr->OnDroped.AddCallback(
					std::bind(&ThisClass::OnWeaponProxyChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
					)
			);
		}
	}

	HumanCharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	EnableMenu();
}

void UTeamMatesList::NativeDestruct()
{
	DelegateAry.Empty();

	Super::NativeDestruct();
}

void UTeamMatesList::EnableMenu()
{
}

void UTeamMatesList::DisEnableMenu()
{
	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(TeanMatesList::Get().VerticalBox));
	if (!PanelPtr)
	{
		return;
	}
	auto ChildrensAry = PanelPtr->GetAllChildren();
	for (int32 Index = 0; Index < ChildrensAry.Num(); Index++)
	{
		auto WidgetPtr = Cast<UTeamMateInfo>(ChildrensAry[Index]);
		if (WidgetPtr)
		{
			WidgetPtr->SynMember2Config(Index);
		}
	}
}

EMenuType UTeamMatesList::GetMenuType() const
{
	return EMenuType::kGroupManagger;
}

void UTeamMatesList::OnWeaponProxyChanged(
	UTeamMateInfo*UIPtr,
	const TSharedPtr<FCharacterProxy>& PrevProxycharacterSPtr,
	const TSharedPtr<FCharacterProxy>& NewProxycharacterSPtr
)
{
	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(TeanMatesList::Get().VerticalBox));
	if (!PanelPtr)
	{
		return;
	}
	auto ChildrensAry = PanelPtr->GetAllChildren();
	for (auto Iter : ChildrensAry)
	{
		auto WidgetPtr = Cast<UTeamMateInfo>(Iter);
		if (WidgetPtr)
		{
			if (UIPtr == WidgetPtr)
			{
				continue;
			}
			if (NewProxycharacterSPtr && (NewProxycharacterSPtr == WidgetPtr->GroupMateProxyPtr))
			{
				{
					TScopeValue ScopeValue(WidgetPtr->bPaseInvokeOnResetProxyEvent, true, false);
					WidgetPtr->ResetToolUIByData(PrevProxycharacterSPtr);
				}
				break;
			}
		}
	}
}

void UTeamMatesList::OnTeammateChanged(UTeamMateInfo* GourpMateProxyPtr)
{
	auto PanelPtr = Cast<UVerticalBox>(GetWidgetFromName(TeanMatesList::Get().VerticalBox));
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
			if (TeamMateInfoPtr == GourpMateProxyPtr)
			{
				continue;
			}
			else
			{
				if (TeamMateInfoPtr->GroupMateProxyPtr == GourpMateProxyPtr->GroupMateProxyPtr)
				{
					TeamMateInfoPtr->ResetToolUIByData(nullptr);
				}
			}
		}
	}
}
