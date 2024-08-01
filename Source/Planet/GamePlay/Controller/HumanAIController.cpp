
#include "HumanAIController.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Components/StateTreeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include <Kismet/GameplayStatics.h>

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

AHumanAIController::AHumanAIController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	//StateTreeComponentPtr = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	StateTreeAIComponentPtr = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
	AIPerceptionComponentPtr = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
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

AActor* AHumanAIController::GetTeamFocusTarget() const
{
	if (GetGroupMnaggerComponent() && GetGroupMnaggerComponent()->GetTeamHelper())
	{
		{
			auto LeaderPCPtr = GetGroupMnaggerComponent()->GetTeamHelper()->OwnerPtr->GetController<APlanetPlayerController>();
			if (LeaderPCPtr)
			{
				return LeaderPCPtr->GetFocusActor();
			}
		}
		{
			auto LeaderPCPtr = GetGroupMnaggerComponent()->GetTeamHelper()->OwnerPtr->GetController<AHumanAIController>();
			if (LeaderPCPtr)
			{
				auto ResultPtr = LeaderPCPtr->GetFocusActor();
				if (ResultPtr)
				{
					return ResultPtr;
				}
				else
				{
					return LeaderPCPtr->TargetCharacterPtr;
				}
			}
		}
	}

	return nullptr;
}

void AHumanAIController::OnTeammateOptionChangedImp(
	ETeammateOption TeammateOption,
	ACharacterBase* LeaderPCPtr
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

		OnTeammateOptionChangedImp(TeamsHelper->GetTeammateOption(), TeamsHelper->OwnerPtr);
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

		auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();
		auto HICPtr = CharacterPtr->GetHoldingItemsComponent();
	}
}
