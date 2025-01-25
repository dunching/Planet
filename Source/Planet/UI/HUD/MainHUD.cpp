#include "MainHUD.h"

#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "Components/Border.h"

#include "MainHUDLayout.h"
#include "UICommon.h"
#include "PlanetPlayerController.h"
#include "GetItemInfosList.h"
#include "InventoryComponent.h"
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
	SwitchLayout(ELayoutCommon::kActionLayout);
}

void AMainHUD::SwitchLayout(ELayoutCommon MainHUDType)
{
	GetMainHUDLayout()->SwitchToNewLayout(MainHUDType);
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
					CharacterPtr->GetInventoryComponent()->OnSkillProxyChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnSkillProxyChanged, ItemInfosPtr, std::placeholders::_1,
						          std::placeholders::_2
						));
				Handle->bIsAutoUnregister = false;
			}
			{
				auto Handle =
					CharacterPtr->GetInventoryComponent()->OnCoinProxyChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnCoinProxyChanged, ItemInfosPtr, std::placeholders::_1,
						          std::placeholders::_2, std::placeholders::_3
						));
				Handle->bIsAutoUnregister = false;
			}
			{
				auto Handle =
					CharacterPtr->GetInventoryComponent()->OnConsumableProxyChanged.AddCallback(
						std::bind(&UGetItemInfosList::OnConsumableProxyChanged, ItemInfosPtr, std::placeholders::_1,
						          std::placeholders::_2
						));
				Handle->bIsAutoUnregister = false;
			}
			{
				auto Handle =
					CharacterPtr->GetInventoryComponent()->OnGroupmateProxyChanged.AddCallback(
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
