
#include "HumanAIController.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Components/StateTreeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Perception/AISenseConfig_Sight.h>

#include "CharacterTitle.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "PlanetPlayerController.h"
#include "TestCommand.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitTable.h"
#include "GeneratorNPCs_Patrol.h"
#include "GeneratorColony.h"
#include "NPCComponent.h"

AHumanAIController::AHumanAIController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	//StateTreeComponentPtr = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	StateTreeAIComponentPtr = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
	AIPerceptionComponentPtr = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// 设置这个之后BP不能保存？
	InitialSenseConfig();
}

void AHumanAIController::InitialSenseConfig()
{
	auto SightConfig = NewObject<UAISenseConfig_Sight>();
	check(SightConfig);
	SightConfig->SightRadius = 50000;
	SightConfig->LoseSightRadius = 53000;
	SightConfig->PeripheralVisionAngleDegrees = 120.f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = FAISystem::InvalidRange;
	SightConfig->SetMaxAge(1.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = 1;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = 1;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = 1;
	AIPerceptionComponentPtr->ConfigureSense(*SightConfig);
}

void AHumanAIController::SetCampType(ECharacterCampType CharacterCampType)
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (CharacterPtr)
	{
		CharacterPtr->SetCampType(CharacterCampType);
	}
}

UAIPerceptionComponent* AHumanAIController::GetAIPerceptionComponent()
{
	return AIPerceptionComponentPtr;
}

bool AHumanAIController::CheckIsFarawayOriginal() const
{
	return false;
}

void AHumanAIController::OnTeammateOptionChangedImp(
	ETeammateOption TeammateOption,
	const TSharedPtr < FCharacterUnitType>& LeaderCharacterUnitPtr
)
{
}

void AHumanAIController::OnDeathing(const FGameplayTag Tag, int32 Count)
{

}

void AHumanAIController::DoDeathing()
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (CharacterPtr)
	{
		if (CharacterPtr->CharacterTitlePtr)
		{
			CharacterPtr->CharacterTitlePtr->RemoveFromParent();
			CharacterPtr->CharacterTitlePtr = nullptr;
		}
	}
}

void AHumanAIController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AHumanAIController::BeginPlay()
{
	Super::BeginPlay();

	GetAIPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
	GetAIPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ThisClass::OnPerceptionUpdated);
}

void AHumanAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoDeathing();

	Super::EndPlay(EndPlayReason);
}

void AHumanAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	InitialAILogic();

	InitialCharacter();
}

void AHumanAIController::OnUnPossess()
{
	if (TeammateOptionChangedDelegate)
	{
		TeammateOptionChangedDelegate->UnBindCallback();
	}

	if (TeamHelperChangedDelegate)
	{
		TeamHelperChangedDelegate->UnBindCallback();
	}

	if (StateTreeAIComponentPtr)
	{
		StateTreeAIComponentPtr->Cleanup();
	}

	Super::OnUnPossess();
}

void AHumanAIController::OnGroupChanged()
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (CharacterPtr)
	{
		auto PlayerCharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (PlayerCharacterPtr)
		{
			SetCampType(
				CharacterPtr->IsTeammate(PlayerCharacterPtr) ? ECharacterCampType::kTeamMate : ECharacterCampType::kEnemy
			);
		}
	}
}

void AHumanAIController::OnTeamChanged()
{
	auto TeamsHelper = GetGroupMnaggerComponent()->GetTeamHelper();
	if (TeamsHelper)
	{
		TeammateOptionChangedDelegate = TeamsHelper->TeammateOptionChanged.AddCallback(
			std::bind(&ThisClass::OnTeammateOptionChangedImp, this, std::placeholders::_1, std::placeholders::_2
			));

		OnTeammateOptionChangedImp(TeamsHelper->GetTeammateOption(), TeamsHelper->OwnerCharacterUnitPtr);
	}
}

void AHumanAIController::InitialCharacter()
{
	auto& DelegateRef = GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		UGameplayTagsSubSystem::GetInstance()->DeathingTag,
		EGameplayTagEventType::NewOrRemoved
	);
	OnOwnedDeathTagDelegateHandle = DelegateRef.AddUObject(this, &ThisClass::OnDeathing);

	GroupHelperChangedDelegate =
		GetGroupMnaggerComponent()->GroupHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnGroupChanged, this));
	OnGroupChanged();

	TeamHelperChangedDelegate =
		GetGroupMnaggerComponent()->TeamHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnTeamChanged, this));
	GetGroupMnaggerComponent()->GetTeamHelper()->SwitchTeammateOption(ETeammateOption::kEnemy);
 
	// 组件自动调用条件不成功，原因未知
 	if (StateTreeAIComponentPtr && !StateTreeAIComponentPtr->IsRunning())
 	{
 		StateTreeAIComponentPtr->StartLogic();
 	}

	auto CharacterPtr = GetPawn<FPawnType>();
	if (CharacterPtr)
	{
#if TESTAICHARACTERHOLDDATA
		TestCommand::AddAICharacterTestDataImp(CharacterPtr);
#endif
		auto HICPtr = CharacterPtr->GetHoldingItemsComponent();
		{
			auto TableRowUnit_CharacterInfoPtr = CharacterPtr->GetCharacterUnit()->GetTableRowUnit_CharacterInfo();
			if (TableRowUnit_CharacterInfoPtr)
			{
				// 武器
				{
					auto EICPtr = CharacterPtr->GetProxyProcessComponent();

					auto FirstWeaponSocketInfoSPtr = MakeShared<FSocket_FASI>();
					if (TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo.IsValid())
					{
						auto WeaponProxyPtr = HICPtr->AddUnit_Weapon(TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo);
						if (WeaponProxyPtr)
						{
							FirstWeaponSocketInfoSPtr->Socket = UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1;
							FirstWeaponSocketInfoSPtr->ProxySPtr = WeaponProxyPtr;
							FirstWeaponSocketInfoSPtr->ProxySPtr->SetAllocationCharacterUnit(CharacterPtr->GetCharacterUnit());
						}
					}
					EICPtr->UpdateSocket(FirstWeaponSocketInfoSPtr);

					auto SecondWeaponSocketInfoSPtr = MakeShared<FSocket_FASI>();
					if (TableRowUnit_CharacterInfoPtr->SecondWeaponSocketInfo.IsValid())
					{
						auto WeaponProxyPtr = HICPtr->AddUnit_Weapon(TableRowUnit_CharacterInfoPtr->SecondWeaponSocketInfo);
						if (WeaponProxyPtr)
						{
							SecondWeaponSocketInfoSPtr->Socket = UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2;
							SecondWeaponSocketInfoSPtr->ProxySPtr = WeaponProxyPtr;
							SecondWeaponSocketInfoSPtr->ProxySPtr->SetAllocationCharacterUnit(CharacterPtr->GetCharacterUnit());
						}
					}
					EICPtr->UpdateSocket(SecondWeaponSocketInfoSPtr);

					EICPtr->ActiveWeapon();
				}

				// 技能
				{
					auto EICPtr = CharacterPtr->GetProxyProcessComponent();

					if (TableRowUnit_CharacterInfoPtr->ActiveSkillSet_1.IsValid())
					{
						auto SkillUnitPtr = HICPtr->AddUnit_Skill(TableRowUnit_CharacterInfoPtr->ActiveSkillSet_1);
						if (SkillUnitPtr)
						{
							auto SkillsSocketInfo = MakeShared<FSocket_FASI>();

							SkillsSocketInfo->Socket = UGameplayTagsSubSystem::GetInstance()->ActiveSocket_1;
							SkillsSocketInfo->ProxySPtr = SkillUnitPtr;
							SkillsSocketInfo->ProxySPtr->SetAllocationCharacterUnit(CharacterPtr->GetCharacterUnit());

							EICPtr->UpdateSocket(SkillsSocketInfo);
						}
					}
					if (TableRowUnit_CharacterInfoPtr->ActiveSkillSet_2.IsValid())
					{
						auto SkillUnitPtr = HICPtr->AddUnit_Skill(TableRowUnit_CharacterInfoPtr->ActiveSkillSet_2);
						if (SkillUnitPtr)
						{
							auto SkillsSocketInfo = MakeShared<FSocket_FASI>();

							SkillsSocketInfo->Socket = UGameplayTagsSubSystem::GetInstance()->ActiveSocket_2;
							SkillsSocketInfo->ProxySPtr = SkillUnitPtr;
							SkillsSocketInfo->ProxySPtr->SetAllocationCharacterUnit(CharacterPtr->GetCharacterUnit());

							EICPtr->UpdateSocket(SkillsSocketInfo);
						}
					}
				}
			}
		}
	}
}

void AHumanAIController::InitialAILogic()
{
	if (GetPawn())
	{
		auto ParentPtr = GetPawn()->GetAttachParentActor();
		if (!ParentPtr)
		{
			return;
		}

		if (auto PatrolPtr = Cast<AGeneratorNPCs_Patrol>(ParentPtr))
		{
			PatrolSPlinePtr = PatrolPtr->SplineComponentPtr;
		}
		else if (auto ColonyPtr = Cast<AGeneratorColony>(ParentPtr))
		{
// 			// 这里按次序获取要追踪的位置
// 			auto NPCComponentPtr = GetPawn()->GetComponentByClass<UNPCComponent>();
// 
// 			PathFollowComponentPtr = NPCComponentPtr->PathFollowComponentPtr;
		}
	}
}
