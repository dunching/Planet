
#include "HumanAIController.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Components/StateTreeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Perception/AISenseConfig_Sight.h>
#include <Components/SplineComponent.h>

#include "CharacterTitle.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "ItemProxy.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "PlanetPlayerController.h"
#include "TestCommand.h"
#include "GameplayTagsSubSystem.h"
#include "BuildingArea.h"
#include "GeneratorNPCs_Patrol.h"
#include "GeneratorColony.h"
#include "HumanCharacter_AI.h"
#include "GroupSharedInfo.h"

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
	SightConfig->SightRadius = 1000;
	SightConfig->LoseSightRadius = 1000;
	SightConfig->PeripheralVisionAngleDegrees = 120.f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = FAISystem::InvalidRange;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = 1;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = 1;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = 1;
	AIPerceptionComponentPtr->ConfigureSense(*SightConfig);
}

void AHumanAIController::InitialAllocations()
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr)
	{
		return;
	}
}

UAIPerceptionComponent* AHumanAIController::GetAIPerceptionComponent()
{
	return AIPerceptionComponentPtr;
}

bool AHumanAIController::CheckIsFarawayOriginal() const
{
	if (BuildingAreaPtr)
	{
		return (FVector::Distance(BuildingAreaPtr->GetActorLocation(), GetPawn()->GetActorLocation()) > 1000);
	}
	
	if (GeneratorNPCs_PatrolPtr)
	{
		auto CharacterPtr = GetPawn<FPawnType>();
		if (CharacterPtr)
		{
			return GeneratorNPCs_PatrolPtr->CheckIsFarawayOriginal(CharacterPtr);
		}
	}

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

	InitialAILogic();

	InitialCharacter();
}

void AHumanAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoDeathing();

	Super::EndPlay(EndPlayReason);
}

void AHumanAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
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
			CharacterPtr->SetCampType(
				CharacterPtr->IsTeammate(PlayerCharacterPtr) ? ECharacterCampType::kTeamMate : ECharacterCampType::kEnemy
			);
		}
	}
}

void AHumanAIController::OnTeamChanged()
{
	auto TeamsHelper = GetGroupSharedInfo()->GetTeamMatesHelperComponent();
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
		auto& DelegateRef = GetAbilitySystemComponent()->RegisterGameplayTagEvent(
			UGameplayTagsSubSystem::DeathingTag,
			EGameplayTagEventType::NewOrRemoved
		);
		OnOwnedDeathTagDelegateHandle = DelegateRef.AddUObject(this, &ThisClass::OnDeathing);

		TeamHelperChangedDelegate =
			GetGroupSharedInfo()->TeamMatesHelperComponentPtr->TeamHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnTeamChanged, this));
		
#if WITH_EDITORONLY_DATA
		GetGroupSharedInfo()->GetTeamMatesHelperComponent()->SwitchTeammateOption(CharacterPtr->DefaultTeammateOption);
#else
		GetGroupSharedInfo()->GetTeamMatesHelperComponent()->SwitchTeammateOption(ETeammateOption::kEnemy);
#endif

		// 组件自动调用条件不成功，原因未知
		if (StateTreeAIComponentPtr && !StateTreeAIComponentPtr->IsRunning())
		{
			StateTreeAIComponentPtr->StartLogic();
		}

		InitialAllocations();
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
			GeneratorNPCs_PatrolPtr = PatrolPtr;
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
