
#include "HumanAIController.h"

#include <GameFramework/CharacterMovementComponent.h>

#include "AIHumanInfo.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "HumanPlayerController.h"

void AHumanAIController::SetCampType(ECharacterCampType CharacterCampType)
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->SetCampType(CharacterCampType);
	}
}

UGroupMnaggerComponent* AHumanAIController::GetGroupMnaggerComponent()const
{
	return GetPawn<FPawnType>()->GetGroupMnaggerComponent();
}

UGourpmateUnit* AHumanAIController::GetGourpMateUnit()
{
	return Cast<FPawnType>(GetPawn())->GetGourpMateUnit(); 
}

AHumanAIController::FPawnType* AHumanAIController::GetCharacter()
{
	return Cast<FPawnType>(GetPawn());
}

AActor* AHumanAIController::GetTeamFocusEnemy() const
{
	if (GetGroupMnaggerComponent() && GetGroupMnaggerComponent()->GetTeamsHelper())
	{
		auto LeaderPCPtr = GetGroupMnaggerComponent()->GetTeamsHelper()->OwnerPCPtr->GetController<AHumanPlayerController>();
		if (LeaderPCPtr)
		{
			return LeaderPCPtr->GetFocusActor();
		}
	}

	return nullptr;
}

void AHumanAIController::OnTeammateOptionChangedImp(
	ETeammateOption TeammateOption,
	FPawnType* LeaderPCPtr
)
{
	OnTeammateOptionChanged(TeammateOption, LeaderPCPtr);
}

void AHumanAIController::BeginPlay()
{
	Super::BeginPlay();

	GetGroupMnaggerComponent();

	TeamHelperChangedDelegateContainer = 
		GetGroupMnaggerComponent()->TeamHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnTeamHelperChanged, this));
}

void AHumanAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->RemoveFromParent();
		AIHumanInfoPtr = nullptr;
	}

	if (TeammateOptionChangedDelegateContainer)
	{
		TeammateOptionChangedDelegateContainer->UnBindCallback();
	}

	if (TeamHelperChangedDelegateContainer)
	{
		TeamHelperChangedDelegateContainer->UnBindCallback();
	}

	Super::EndPlay(EndPlayReason);
}

void AHumanAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn); 

	auto CharacterPtr = Cast<FPawnType>(InPawn);
	if (CharacterPtr)
	{
		auto AssetRefMapPtr = UAssetRefMap::GetInstance();
		AIHumanInfoPtr = CreateWidget<UAIHumanInfo>(GetWorldImp(), AssetRefMapPtr->AIHumanInfoClass);
		if (AIHumanInfoPtr)
		{
			AIHumanInfoPtr->CharacterPtr = CharacterPtr;
			AIHumanInfoPtr->AddToViewport();
		}

		auto EICPtr = CharacterPtr->GetEquipmentItemsComponent();
		auto HICPtr = CharacterPtr->GetHoldingItemsComponent();
		{
			auto WeaponUnitPtr = HICPtr->GetHoldItemProperty().FindUnit(EWeaponUnitType::kPickAxe);
			EICPtr->SetMainWeapon(WeaponUnitPtr);

			TMap<FGameplayTag, FSkillsSocketInfo> InSkillsMap;
			{
				FSkillsSocketInfo SkillsSocketInfo;

				SkillsSocketInfo.SkillSocket = FGameplayTag::RequestGameplayTag(TEXT("UI.SkillSocket.WeaponActiveSocket1"));
				SkillsSocketInfo.SkillUnit = HICPtr->GetHoldItemProperty().AddUnit(WeaponUnitPtr->FirstSkillClass);

				InSkillsMap.Add(
					SkillsSocketInfo.SkillSocket,
					SkillsSocketInfo
				);
			}

			EICPtr->RegisterMultiGAs(InSkillsMap);

			EICPtr->ActiveWeapon(EWeaponSocket::kMain);
		}
		CharacterPtr->SwitchAnimLink(EAnimLinkClassType::kUnarmed);
	}

	SetCampType(ECharacterCampType::kEnemy);
}

void AHumanAIController::OnTeamHelperChanged()
{
	auto TeamsHelper = GetGroupMnaggerComponent()->GetTeamsHelper();
	if (TeamsHelper)
	{
		TeammateOptionChangedDelegateContainer = TeamsHelper->TeammateOptionChanged.AddCallback(
			std::bind(&ThisClass::OnTeammateOptionChangedImp, this, std::placeholders::_1, std::placeholders::_2
			));

		auto PlayerPCPtr = Cast<AHumanPlayerController>(TeamsHelper->OwnerPCPtr);
		if (PlayerPCPtr)
		{
			OnTeammateOptionChangedImp(TeamsHelper->GetTeammateOption(), TeamsHelper->OwnerPCPtr);
		}
	}
}
