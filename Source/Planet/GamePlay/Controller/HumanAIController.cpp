
#include "HumanAIController.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Components/StateTreeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include <Kismet/GameplayStatics.h>

#include "AIHumanInfo.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "HumanPlayerController.h"
#include "TestCommand.h"

AHumanAIController::AHumanAIController(const FObjectInitializer& ObjectInitializer) :
	Super()
{
	StateTreeComponentPtr = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
}

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
	return  GetPawn<FPawnType>()->GetGourpMateUnit();
}

AActor* AHumanAIController::GetTeamFocusEnemy() const
{
	if (GetGroupMnaggerComponent() && GetGroupMnaggerComponent()->GetTeamHelper())
	{
		auto LeaderPCPtr = GetGroupMnaggerComponent()->GetTeamHelper()->OwnerPCPtr->GetController<AHumanPlayerController>();
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
}

void AHumanAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->RemoveFromParent();
		AIHumanInfoPtr = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AHumanAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn); 

	InitialCharacter();

	GroupHelperChangedDelegate =
		GetGroupMnaggerComponent()->GroupHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnGroupChanged, this));
	OnGroupChanged();

	TeamHelperChangedDelegate =
		GetGroupMnaggerComponent()->TeamHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnTeamChanged, this));
	OnTeamChanged();

	if (StateTreeComponentPtr && !StateTreeComponentPtr->IsRunning())
	{
		StateTreeComponentPtr->StartLogic(); 
	}
}

void AHumanAIController::OnUnPossess()
{
	if (TeammateOptionChangedDelegateContainer)
	{
		TeammateOptionChangedDelegateContainer->UnBindCallback();
	}

	if (TeamHelperChangedDelegate)
	{
		TeamHelperChangedDelegate->UnBindCallback();
	}

	Super::OnUnPossess();
}

void AHumanAIController::OnGroupChanged()
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (CharacterPtr)
	{
		SetCampType(
			CharacterPtr->IsTeammate(Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0))) ? ECharacterCampType::kTeamMate : ECharacterCampType::kEnemy
		);
	}
}

void AHumanAIController::OnTeamChanged()
{
	auto TeamsHelper = GetGroupMnaggerComponent()->GetTeamHelper();
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

void AHumanAIController::InitialCharacter()
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (CharacterPtr)
	{
#if TESTHOLDDATA
		TestCommand::AddAICharacterTestDataImp(CharacterPtr);
#endif

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
			if (WeaponUnitPtr)
			{
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
	}
}
