
#include "MainHUD.h"

#include "Components/Border.h"

#include "MainHUDLayout.h"
#include "UICommon.h"
#include "PlanetPlayerController.h"
#include "GetItemInfosList.h"
#include "HoldingItemsComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "CharacterBase.h"
#include "GameOptions.h"
#include "RegularActionLayout.h"
#include "EndangeredStateLayout.h"

struct FMainHUD : public TStructVariable<FMainHUD>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName FocusCharacterSocket = TEXT("FocusCharacterSocket");
};

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	InitMainHUDLayout();
	SwitchState(EMainHUDType::kRegularAction);
}

void AMainHUD::ShowHUD()
{
	Super::ShowHUD();
}

void AMainHUD::SwitchState(EMainHUDType MainHUDType)
{
	if (bShowHUD && PlayerOwner)
	{
		switch (MainHUDType)
		{
		default:
		{
			if (EndangeredStatePtr)
			{
				EndangeredStatePtr->RemoveFromParent();
				EndangeredStatePtr = nullptr;
			}
			if (RegularActionStatePtr)
			{
				RegularActionStatePtr->RemoveFromParent();
				RegularActionStatePtr = nullptr;
			}
		}
		break;
		case EMainHUDType::kRegularAction:
		{
			if (EndangeredStatePtr)
			{
				EndangeredStatePtr->RemoveFromParent();
				EndangeredStatePtr = nullptr;
			}

			if (!RegularActionStatePtr)
			{
				RegularActionStatePtr = CreateWidget<URegularActionLayout>(GetWorld(), RegularActionStateClass);
				if (RegularActionStatePtr)
				{
					RegularActionStatePtr->AddToViewport(EUIOrder::kHUD);
				}
			}
		}
		break;
		case EMainHUDType::kEndangered:
		{
			if (RegularActionStatePtr)
			{
				RegularActionStatePtr->RemoveFromParent();
				RegularActionStatePtr = nullptr;
			}

			if (!EndangeredStatePtr)
			{
				EndangeredStatePtr = CreateWidget<UEndangeredStateLayout>(GetWorld(), EndangeredStateClass);
				if (EndangeredStatePtr)
				{
					EndangeredStatePtr->AddToViewport(EUIOrder::kHUD);
				}
			}
		}
		break;
		}
	}
}

void AMainHUD::OnHPChanged(int32 InCurrentValue)
{
	if (bShowHUD && PlayerOwner)
	{
		if (PlayerOwner->IsA(APlanetPlayerController::StaticClass()))
		{
			auto PCPtr = Cast<APlanetPlayerController>(PlayerOwner);
			if (!PCPtr)
			{
				return;
			}

			auto& CharacterAttributesRef =
				PCPtr->GetPawn<ACharacterBase>()->GetCharacterAttributesComponent()->GetCharacterAttributes();

			const auto CurrentValue =
				CharacterAttributesRef.HP.GetCurrentValue();
			const auto MaxValue =
				CharacterAttributesRef.HP.GetMaxValue();
			const auto LowerHP_Percent =
				UGameOptions::GetInstance()->LowerHP_Percent;

			MainHUDLayoutPtr->SwitchIsLowerHP(CurrentValue < (MaxValue * (LowerHP_Percent / 100.f)));
		}
	}
}

void AMainHUD::InitMainHUDLayout()
{
	if (bShowHUD && PlayerOwner)
	{
		MainHUDLayoutPtr = CreateWidget<UMainHUDLayout>(GetWorld(), MainHUDLayoutClass);
		if (MainHUDLayoutPtr)
		{
			MainHUDLayoutPtr->AddToViewport(EUIOrder::kHUD);
		}

		if (PlayerOwner->IsA(APlanetPlayerController::StaticClass()))
		{
			auto PCPtr = Cast<APlanetPlayerController>(PlayerOwner);
			if (!PCPtr)
			{
				return;
			}

			{
				auto& CharacterAttributesRef =
					PCPtr->GetPawn<ACharacterBase>()->GetCharacterAttributesComponent()->GetCharacterAttributes();

				auto Handle =
					CharacterAttributesRef.HP.AddOnValueChanged(
						std::bind(&ThisClass::OnHPChanged, this, std::placeholders::_2)
					);
				Handle->bIsAutoUnregister = false;

				OnHPChanged(
					CharacterAttributesRef.HP.GetCurrentValue()
				);
			}

			auto ItemInfosPtr = MainHUDLayoutPtr->GetItemInfos();
			{
				auto Handle =
					PCPtr->GetHoldingItemsComponent()->OnSkillUnitChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnSkillUnitChanged, ItemInfosPtr, std::placeholders::_1, std::placeholders::_2
						));
				Handle->bIsAutoUnregister = false;
			}
			{
				auto Handle =
					PCPtr->GetHoldingItemsComponent()->OnCoinUnitChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnCoinUnitChanged, ItemInfosPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
						));
				Handle->bIsAutoUnregister = false;
			}
			{
				auto Handle =
					PCPtr->GetHoldingItemsComponent()->OnConsumableUnitChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnConsumableUnitChanged, ItemInfosPtr, std::placeholders::_1, std::placeholders::_2
						));
				Handle->bIsAutoUnregister = false;
			}
			{
				auto Handle =
					PCPtr->GetHoldingItemsComponent()->OnGroupmateUnitChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnGourpmateUnitChanged, ItemInfosPtr, std::placeholders::_1, std::placeholders::_2
						));
				Handle->bIsAutoUnregister = false;
			}
#if TESTPLAYERCHARACTERHOLDDATA
			TestCommand::AddPlayerCharacterTestDataImp(this);
#endif
		}
	}
	else
	{
		if (MainHUDLayoutPtr)
		{
			MainHUDLayoutPtr->RemoveFromParent();
			MainHUDLayoutPtr = nullptr;
		}
	}
}
