
#include "PlanetAIController.h"

#include <GameFramework/CharacterMovementComponent.h>

#include "AIHumanInfo.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "PlanetPlayerController.h"

void APlanetAIController::SetCampType(ECharacterCampType CharacterCampType)
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->SetCampType(CharacterCampType);
	}
}

UGroupMnaggerComponent* APlanetAIController::GetGroupMnaggerComponent()
{
	if (!GroupMnaggerComponentPtr)
	{
		GroupMnaggerComponentPtr = Cast<UGroupMnaggerComponent>(AddComponentByClass(UGroupMnaggerComponent::StaticClass(), true, FTransform::Identity, false));
	}
	return  GroupMnaggerComponentPtr;
}

UGourpmateUnit* APlanetAIController::GetGourpMateUnit()
{
	return Cast<AHumanCharacter>(GetPawn())->GetGourpMateUnit();
}

ACharacterBase* APlanetAIController::GetCharacter()
{
	return Cast<ACharacterBase>(GetPawn());
}

AActor* APlanetAIController::GetTeamFocusEnemy() const
{
	if (GroupMnaggerComponentPtr && GroupMnaggerComponentPtr->GetTeamsHelper())
	{
		auto LeaderPCPtr = Cast<APlanetPlayerController>(GroupMnaggerComponentPtr->GetTeamsHelper()->OwnerPCPtr);
		if (LeaderPCPtr)
		{
			return LeaderPCPtr->GetFocusActor();
		}
	}

	return nullptr;
}

void APlanetAIController::OnTeammateOptionChangedImp(
	ETeammateOption TeammateOption,
	IPlanetControllerInterface* LeaderPCPtr
)
{
	OnTeammateOptionChanged(TeammateOption, LeaderPCPtr->GetCharacter());
}

void APlanetAIController::BeginPlay()
{
	Super::BeginPlay();

	GetGroupMnaggerComponent();

	TeamHelperChangedDelegateContainer = 
		GroupMnaggerComponentPtr->TeamHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnTeamHelperChanged, this));
}

void APlanetAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void APlanetAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	auto CharacterPtr = Cast<ACharacterBase>(InPawn);
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
	}

	SetCampType(ECharacterCampType::kEnemy);
}

void APlanetAIController::OnTeamHelperChanged()
{
	auto TeamsHelper = GroupMnaggerComponentPtr->GetTeamsHelper();
	if (TeamsHelper)
	{
		TeammateOptionChangedDelegateContainer = TeamsHelper->TeammateOptionChanged.AddCallback(
			std::bind(&ThisClass::OnTeammateOptionChangedImp, this, std::placeholders::_1, std::placeholders::_2
			));

		auto PlayerPCPtr = Cast<APlanetPlayerController>(TeamsHelper->OwnerPCPtr);
		if (PlayerPCPtr)
		{
			OnTeammateOptionChangedImp(TeamsHelper->GetTeammateOption(), TeamsHelper->OwnerPCPtr);
		}
	}
}
