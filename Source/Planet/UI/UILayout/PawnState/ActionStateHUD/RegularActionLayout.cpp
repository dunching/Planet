
#include "RegularActionLayout.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Border.h"

#include "TemplateHelper.h"
#include "PlanetPlayerController.h"
#include "UIManagerSubSystem.h"
#include "EffectsList.h"
#include "CharacterBase.h"
#include "FocusIcon.h"
#include "AssetRefMap.h"
#include "UICommon.h"
#include "FocusTitle.h"
#include "HUD_TeamInfo.h"
#include "ProgressTips.h"

struct FRegularActionLayout : public TStructVariable<FRegularActionLayout>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName FocusCharacterSocket = TEXT("FocusCharacterSocket");
};

void URegularActionLayout::NativeConstruct()
{
	Super::NativeConstruct();

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PCPtr)
	{
		// 绑定效果状态栏
		auto EffectPtr = ViewEffectsList(true);
		if (EffectPtr)
		{
			EffectPtr->BindCharacterState(PCPtr->GetPawn<ACharacterBase>());
		}

		DisplayTeamInfo(false);
		ViewEffectsList(false);
		ViewProgressTips(false);

		OnFocusCharacter(nullptr);
		auto DelegateHandle =
			PCPtr->OnFocusCharacterDelegate.AddCallback(std::bind(&ThisClass::OnFocusCharacter, this, std::placeholders::_1));
		DelegateHandle->bIsAutoUnregister = false;
	}
}

void URegularActionLayout::OnFocusCharacter(ACharacterBase* TargetCharacterPtr)
{
	// 
	if (TargetCharacterPtr)
	{
		auto AssetRefMapPtr = UAssetRefMap::GetInstance();
		FocusIconPtr = CreateWidget<UFocusIcon>(GetWorldImp(), AssetRefMapPtr->FocusIconClass);
		if (FocusIconPtr)
		{
			FocusIconPtr->TargetCharacterPtr = TargetCharacterPtr;
			FocusIconPtr->AddToViewport(EUIOrder::kFocus);
		}
	}
	else
	{
		if (FocusIconPtr)
		{
			FocusIconPtr->RemoveFromParent();
			FocusIconPtr = nullptr;
		}
	}

	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FRegularActionLayout::Get().FocusCharacterSocket));
	if (!BorderPtr)
	{
		return;
	}

	if (TargetCharacterPtr)
	{
		// 
		UFocusTitle* UIPtr = nullptr;
		for (auto Iter : BorderPtr->GetAllChildren())
		{
			UIPtr = Cast<UFocusTitle>(Iter);
			if (UIPtr)
			{
				break;
			}
		}
		if (!UIPtr)
		{
			UIPtr = CreateWidget<UFocusTitle>(GetWorldImp(), FocusTitleClass);
			BorderPtr->AddChild(UIPtr);
		}
		if (UIPtr)
		{
			UIPtr->SetTargetCharacter(TargetCharacterPtr);
		}
	}
	else
	{
		BorderPtr->ClearChildren();
	}
}

UEffectsList* URegularActionLayout::ViewEffectsList(bool bIsViewMenus)
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(EffectsListSocket));
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
		auto UIPtr = CreateWidget<UEffectsList>(GetWorldImp(), EffectsListClass);
		if (UIPtr)
		{
			UIPtr->bIsPositiveSequence = false;
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

void URegularActionLayout::DisplayTeamInfo(bool bIsDisplay, AHumanCharacter* HumanCharacterPtr)
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(HUD_TeamSocket));
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
			UIPtr = CreateWidget<UHUD_TeamInfo>(GetWorldImp(), HUD_TeamInfoClass);
			if (UIPtr)
			{
				BorderPtr->AddChild(UIPtr);
			}
		}
	}
}

UProgressTips* URegularActionLayout::ViewProgressTips(bool bIsViewMenus)
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(ProgressTipsSocket));
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

		auto UIPtr = CreateWidget<UProgressTips>(GetWorldImp(), ProgressTipsClass);
		if (UIPtr)
		{
			BorderPtr->AddChild(UIPtr);

			return UIPtr;
		}
	}

	BorderPtr->ClearChildren();

	return nullptr;
}
