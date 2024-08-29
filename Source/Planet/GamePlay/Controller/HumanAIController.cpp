
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
		if (CharacterPtr->CharacterTitlePtr)
		{
			CharacterPtr->CharacterTitlePtr->SetCampType(CharacterCampType);
		}
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
	FCharacterUnitType* LeaderCharacterUnitPtr
)
{
}

void AHumanAIController::OnDeathing(const FGameplayTag Tag, int32 Count)
{
	if (Count > 0)
	{
		GetAbilitySystemComponent()->UnregisterGameplayTagEvent(
			OnOwnedDeathTagDelegateHandle,
			UGameplayTagsSubSystem::GetInstance()->DeathingTag,
			EGameplayTagEventType::NewOrRemoved
		);

		DoDeathing();
	}
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

	InitialCharacter();

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

	if (StateTreeAIComponentPtr && !StateTreeAIComponentPtr->IsRunning())
	{
		StateTreeAIComponentPtr->StartLogic();
	}
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
					TSharedPtr<FWeaponSocketInfo > FirstWeaponSocketInfoSPtr = MakeShared<FWeaponSocketInfo>();
					{
						auto WeaponUnitPtr = Cast<UWeaponUnit>(HICPtr->AddUnit(TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo, 1));
						if (WeaponUnitPtr)
						{
							FirstWeaponSocketInfoSPtr->WeaponSocket = UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1;
							FirstWeaponSocketInfoSPtr->WeaponUnitPtr = WeaponUnitPtr;
						}
					}

					TSharedPtr<FWeaponSocketInfo > SecondWeaponSocketInfoSPtr = MakeShared<FWeaponSocketInfo>();
					{
						auto WeaponUnitPtr = Cast<UWeaponUnit>(HICPtr->AddUnit(TableRowUnit_CharacterInfoPtr->SecondWeaponSocketInfo, 1));
						if (WeaponUnitPtr)
						{
							SecondWeaponSocketInfoSPtr->WeaponSocket = UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2;
							SecondWeaponSocketInfoSPtr->WeaponUnitPtr = WeaponUnitPtr;
						}
					}

					auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();
					EICPtr->RegisterWeapon(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
					EICPtr->ActiveWeapon(EWeaponSocket::kMain);
				}

				// 技能
				{
					TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>> SkillsMap;
					{
						auto SkillUnitPtr = Cast<USkillUnit>(HICPtr->AddUnit(TableRowUnit_CharacterInfoPtr->ActiveSkillSet_1, 1));
						if (SkillUnitPtr)
						{
							TSharedPtr<FSkillSocketInfo >SkillsSocketInfo = MakeShared<FSkillSocketInfo>();

							SkillsSocketInfo->SkillSocket = UGameplayTagsSubSystem::GetInstance()->ActiveSocket1;
							SkillsSocketInfo->SkillUnitPtr = SkillUnitPtr;

							SkillsMap.Add(SkillsSocketInfo->SkillSocket, SkillsSocketInfo);
						}
					}
					{
						auto SkillUnitPtr = Cast<USkillUnit>(HICPtr->AddUnit(TableRowUnit_CharacterInfoPtr->ActiveSkillSet_2, 1));
						if (SkillUnitPtr)
						{
							TSharedPtr<FSkillSocketInfo >SkillsSocketInfo = MakeShared<FSkillSocketInfo>();

							SkillsSocketInfo->SkillSocket = UGameplayTagsSubSystem::GetInstance()->ActiveSocket2;
							SkillsSocketInfo->SkillUnitPtr = SkillUnitPtr;

							SkillsMap.Add(SkillsSocketInfo->SkillSocket, SkillsSocketInfo);
						}
					}
					auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();
					EICPtr->RegisterMultiGAs(SkillsMap);
				}
			}
		}
	}
}
