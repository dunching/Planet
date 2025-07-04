#include "MainHUD.h"

#include "AssetRefMap.h"
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
#include "MinimapSceneCapture2D.h"
#include "Kismet/GameplayStatics.h"

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
	SwitchLayout(ELayoutCommon::kActionLayout, nullptr);

	if (PlayerOwner && PlayerOwner->GetPawn())
	{
		auto MinimapSceneCapture2DPtr = UGameplayStatics::GetActorOfClass(this, AMinimapSceneCapture2D::StaticClass());
		if (MinimapSceneCapture2DPtr)
		{
			MinimapSceneCapture2DPtr->AttachToActor(
													PlayerOwner->GetPawn(),
													FAttachmentTransformRules::KeepRelativeTransform
												   );
			MinimapSceneCapture2DPtr->SetActorRelativeLocation(FVector::ZeroVector);
			MinimapSceneCapture2DPtr->SetActorRotation(FRotator::ZeroRotator);
		}
	}
}

void AMainHUD::SwitchLayout(
	ELayoutCommon MainHUDType,
	const ILayoutInterfacetion::FOnQuit& OnQuit
	)
{
	GetMainHUDLayout()->SwitchToNewLayout(MainHUDType, OnQuit);
}

UMainHUDLayout* AMainHUD::GetMainHUDLayout() const
{
	return MainHUDLayoutPtr;
}

void AMainHUD::OnHPChanged(
	const FOnAttributeChangeData&


	
	)
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
			ItemInfosPtr->SetPlayerCharacter(CharacterPtr);
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
