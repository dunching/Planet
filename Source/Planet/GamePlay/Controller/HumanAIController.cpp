
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
	//StateTreeComponentPtr = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	StateTreeAIComponentPtr = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
}

void AHumanAIController::SetCampType(ECharacterCampType CharacterCampType)
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->SetCampType(CharacterCampType);
	}
}

UPlanetAbilitySystemComponent* AHumanAIController::GetAbilitySystemComponent() const
{
	return GetPawn<FPawnType>()->GetAbilitySystemComponent();
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

void AHumanAIController::OnDeathing(const FGameplayTag Tag, int32 Count)
{
	if (Count > 0)
	{
		GetAbilitySystemComponent()->UnregisterGameplayTagEvent(
			OnOwnedDeathTagDelegateHandle,
			UAssetRefMap::GetInstance()->DeathingTag,
			EGameplayTagEventType::NewOrRemoved
		);

		DoDeathing();
	}
}

void AHumanAIController::DoDeathing()
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->RemoveFromParent();
		AIHumanInfoPtr = nullptr;
	}
}

void AHumanAIController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AHumanAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AHumanAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoDeathing();

	Super::EndPlay(EndPlayReason);
}

void AHumanAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InitialCharacter();

	auto& DelegateRef = GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		UAssetRefMap::GetInstance()->DeathingTag,
		EGameplayTagEventType::NewOrRemoved
	);
	OnOwnedDeathTagDelegateHandle = DelegateRef.AddUObject(this, &ThisClass::OnDeathing);

	GroupHelperChangedDelegate =
		GetGroupMnaggerComponent()->GroupHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnGroupChanged, this));
	OnGroupChanged();

	TeamHelperChangedDelegate =
		GetGroupMnaggerComponent()->TeamHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnTeamChanged, this));
	OnTeamChanged();

	if (StateTreeAIComponentPtr && !StateTreeAIComponentPtr->IsRunning())
	{
		StateTreeAIComponentPtr->StartLogic();
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
			// ÎäÆ÷
			TSharedPtr<FWeaponSocketInfo> FirstWeaponSocketInfoSPtr = MakeShared<FWeaponSocketInfo>();
			{
				auto WeaponUnitPtr = HICPtr->GetHoldItemProperty().FindUnit(EWeaponUnitType::kPickAxe);
				if (WeaponUnitPtr)
				{
					FirstWeaponSocketInfoSPtr->WeaponSocket = UAssetRefMap::GetInstance()->WeaponActiveSocket1;
					FirstWeaponSocketInfoSPtr->WeaponUnitPtr = WeaponUnitPtr;
				}
			}
			TSharedPtr < FWeaponSocketInfo >SecondWeaponSocketInfo = MakeShared<FWeaponSocketInfo>();
			{
				auto WeaponUnitPtr = HICPtr->GetHoldItemProperty().FindUnit(EWeaponUnitType::kRangeTest);
				if (WeaponUnitPtr)
				{
					SecondWeaponSocketInfo->WeaponSocket = UAssetRefMap::GetInstance()->WeaponActiveSocket2;
					SecondWeaponSocketInfo->WeaponUnitPtr = WeaponUnitPtr;
				}
			}
			EICPtr->RegisterWeapon(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfo);
			EICPtr->SwitchWeapon();
		}
		// ¼¼ÄÜ
		{
			TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>> SkillsMap;
			{
				TSharedPtr < FSkillSocketInfo> SkillsSocketInfo = MakeShared<FSkillSocketInfo>();

				SkillsSocketInfo->SkillSocket = UAssetRefMap::GetInstance()->WeaponActiveSocket1;
				SkillsSocketInfo->SkillUnit = HICPtr->GetHoldItemProperty().FindUnit(ESkillUnitType::kHumanSkill_Active_Displacement);

				SkillsMap.Add(
					SkillsSocketInfo->SkillSocket,
					SkillsSocketInfo
				);
			}
			{
				TSharedPtr < FSkillSocketInfo> SkillsSocketInfo = MakeShared<FSkillSocketInfo>();

				SkillsSocketInfo->SkillSocket = UAssetRefMap::GetInstance()->WeaponActiveSocket2;
				SkillsSocketInfo->SkillUnit = HICPtr->GetHoldItemProperty().FindUnit(ESkillUnitType::kHumanSkill_Active_ContinuousGroupTherapy);

				SkillsMap.Add(
					SkillsSocketInfo->SkillSocket,
					SkillsSocketInfo
				);
			}
			EICPtr->RegisterMultiGAs(SkillsMap);
		}
		EICPtr->GenerationCanbeActivedInfo();
	}
}
