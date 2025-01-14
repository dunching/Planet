#include "MainHUD.h"

#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
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
}

void AMainHUD::ShowHUD()
{
	Super::ShowHUD();
}

void AMainHUD::InitalHUD()
{
	InitMainHUDLayout();
	SwitchState(EMainHUDType::kRegularAction);
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
		case EMainHUDType::kNone:
			{
				if (RegularActionStatePtr)
				{
					RegularActionStatePtr->RemoveFromParent();
					RegularActionStatePtr = nullptr;
				}

				if (EndangeredStatePtr)
				{
					EndangeredStatePtr->RemoveFromParent();
					EndangeredStatePtr = nullptr;
				}
			}
			break;
		}
	}
}

UMainHUDLayout* AMainHUD::GetMainHUDLayout() const
{
	return MainHUDLayoutPtr;
}

void AMainHUD::OnHPChanged(const FOnAttributeChangeData&)
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

			auto CharacterAttributesRef =
				PCPtr->GetPawn<ACharacterBase>()->GetCharacterAttributesComponent()->GetCharacterAttributes();

			const auto CurrentValue =
				CharacterAttributesRef->GetHP();
			const auto MaxValue =
				CharacterAttributesRef->GetMax_HP();
			const auto LowerHP_Percent =
				UGameOptions::GetInstance()->LowerHP_Percent;

			MainHUDLayoutPtr->SwitchIsLowerHP(CurrentValue < (MaxValue * (LowerHP_Percent / 100.f)));
		}
	}
}

void AMainHUD::OnHPChangedImp()
{
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
			auto CharacterPtr = Cast<APlanetPlayerController>(PlayerOwner)->GetPawn<ACharacterBase>();
			if (!CharacterPtr)
			{
				return;
			}

			{
				auto CharacterAttributesRef =
					CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				CharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
					CharacterAttributesRef->GetMax_HPAttribute()
				).AddUObject(this, &ThisClass::OnHPChanged);

				CharacterPtr->GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
					CharacterAttributesRef->GetHPAttribute()
				).AddUObject(this, &ThisClass::OnHPChanged);

				OnHPChangedImp();
			}

			auto ItemInfosPtr = MainHUDLayoutPtr->GetItemInfos();
			{
				auto Handle =
					CharacterPtr->GetHoldingItemsComponent()->OnSkillProxyChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnSkillProxyChanged, ItemInfosPtr, std::placeholders::_1,
						          std::placeholders::_2
						));
				Handle->bIsAutoUnregister = false;
			}
			{
				auto Handle =
					CharacterPtr->GetHoldingItemsComponent()->OnCoinProxyChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnCoinProxyChanged, ItemInfosPtr, std::placeholders::_1,
						          std::placeholders::_2, std::placeholders::_3
						));
				Handle->bIsAutoUnregister = false;
			}
			{
				auto Handle =
					CharacterPtr->GetHoldingItemsComponent()->OnConsumableProxyChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnConsumableProxyChanged, ItemInfosPtr, std::placeholders::_1,
						          std::placeholders::_2
						));
				Handle->bIsAutoUnregister = false;
			}
			{
				auto Handle =
					CharacterPtr->GetHoldingItemsComponent()->OnGroupmateProxyChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnGourpmateProxyChanged, ItemInfosPtr, std::placeholders::_1,
						          std::placeholders::_2
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
