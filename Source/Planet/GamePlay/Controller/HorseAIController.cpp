
#include "HorseAIController.h"

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
#include "UICommon.h"
#include "HorseCharacter.h"

AHorseAIController::AHorseAIController(const FObjectInitializer& ObjectInitializer) :
	Super()
{
	//StateTreeComponentPtr = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	StateTreeAIComponentPtr = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
	AIPerceptionComponentPtr = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

void AHorseAIController::SetCampType(ECharacterCampType CharacterCampType)
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->SetCampType(CharacterCampType);
	}
}

UAIPerceptionComponent* AHorseAIController::GetAIPerceptionComponent()
{
	return AIPerceptionComponentPtr;
}

AActor* AHorseAIController::GetTeamFocusTarget() const
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
			auto LeaderPCPtr = GetGroupMnaggerComponent()->GetTeamHelper()->OwnerPtr->GetController<AHorseAIController>();
			if (LeaderPCPtr)
			{
				auto ResultPtr = LeaderPCPtr->GetFocusActor();
				if (ResultPtr)
				{
					return ResultPtr;
				}
				else
				{
					return LeaderPCPtr->TargetCharacterPtr.Get();
				}
			}
		}
	}

	return nullptr;
}

void AHorseAIController::OnTeammateOptionChangedImp(
	ETeammateOption TeammateOption,
	ACharacterBase* LeaderPCPtr
)
{
	OnTeammateOptionChanged(TeammateOption, LeaderPCPtr);
}

void AHorseAIController::OnDeathing(const FGameplayTag Tag, int32 Count)
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

void AHorseAIController::DoDeathing()
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->RemoveFromParent();
		AIHumanInfoPtr = nullptr;
	}
}

void AHorseAIController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AHorseAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AHorseAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoDeathing();

	Super::EndPlay(EndPlayReason);
}

void AHorseAIController::OnPossess(APawn* InPawn)
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

void AHorseAIController::OnUnPossess()
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

void AHorseAIController::OnGroupChanged()
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (CharacterPtr)
	{
		SetCampType(
			CharacterPtr->IsTeammate(Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0))) ? ECharacterCampType::kTeamMate : ECharacterCampType::kEnemy
		);
	}
}

void AHorseAIController::OnTeamChanged()
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

void AHorseAIController::InitialCharacter()
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (CharacterPtr)
	{
#if TESTAICHARACTERHOLDDATA
//		TestCommand::AddAICharacterTestDataImp(CharacterPtr);
#endif

		auto AssetRefMapPtr = UAssetRefMap::GetInstance();
		AIHumanInfoPtr = CreateWidget<UCharacterTitle>(GetWorldImp(), AssetRefMapPtr->AIHumanInfoClass);
		if (AIHumanInfoPtr)
		{
			AIHumanInfoPtr->CharacterPtr = CharacterPtr;
			AIHumanInfoPtr->AddToViewport(EUIOrder::kCharacter_State_HUD);
		}

		auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();
		auto HICPtr = CharacterPtr->GetHoldingItemsComponent();

	}
}
