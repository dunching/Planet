
#include "HorseAIController.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Components/StateTreeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include <Kismet/GameplayStatics.h>

#include "CharacterTitle.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "PlanetModule.h"
#include "TeamMatesHelperComponentBase.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter.h"
#include "InventoryComponent.h"
#include "PlanetPlayerController.h"
#include "GameplayTagsLibrary.h"
#include "UICommon.h"
#include "HorseCharacter.h"
#include "GroupManagger.h"
#include "TeamMatesHelperComponent.h"
#include "Tools.h"

AHorseAIController::AHorseAIController(const FObjectInitializer& ObjectInitializer) :
                                                                                    Super(ObjectInitializer)
{
	//StateTreeComponentPtr = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	StateTreeAIComponentPtr = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
	AIPerceptionComponentPtr = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

void AHorseAIController::SetGroupSharedInfo(AGroupManagger* InGroupSharedInfoPtr)
{
	Super::SetGroupSharedInfo(InGroupSharedInfoPtr);
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

void AHorseAIController::OnTeammateOptionChangedImp(
	ETeammateOption TeammateOption,
	const TSharedPtr<FCharacterProxyType>& LeaderPCPtr
)
{
}

void AHorseAIController::OnDeathing(const FGameplayTag Tag, int32 Count)
{
	if (Count > 0)
	{
		GetAbilitySystemComponent()->UnregisterGameplayTagEvent(
			OnOwnedDeathTagDelegateHandle,
			UGameplayTagsLibrary::State_Dying,
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
		UGameplayTagsLibrary::State_Dying,
		EGameplayTagEventType::NewOrRemoved
	);
	OnOwnedDeathTagDelegateHandle = DelegateRef.AddUObject(this, &ThisClass::OnDeathing);

	TeamHelperChangedDelegate =
		GetGroupManagger()->GetTeamMatesHelperComponent()->TeamHelperChangedDelegateContainer.AddCallback(std::bind(&ThisClass::OnTeamChanged, this));
	GetGroupManagger()->GetTeamMatesHelperComponent()->SwitchTeammateOption(ETeammateOption::kEnemy);

	if (StateTreeAIComponentPtr && !StateTreeAIComponentPtr->IsRunning())
	{
		StateTreeAIComponentPtr->StartLogic();
	}
}

void AHorseAIController::OnUnPossess()
{
	if (TeammateOptionChangedDelegate)
	{
		TeammateOptionChangedDelegate->UnBindCallback();
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
	auto TeamsHelper = GetGroupManagger()->GetTeamMatesHelperComponent();
	if (TeamsHelper)
	{
		TeammateOptionChangedDelegate = TeamsHelper->TeammateOptionChanged.AddCallback(
			std::bind(&ThisClass::OnTeammateOptionChangedImp, this, std::placeholders::_1, std::placeholders::_2
			));

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
			// AIHumanInfoPtr->CharacterPtr = CharacterPtr;
			AIHumanInfoPtr->AddToViewport(EUIOrder::kOtherPlayer_Character_State_Title);
		}

		auto EICPtr = CharacterPtr->GetProxyProcessComponent();
		auto HICPtr = CharacterPtr->GetInventoryComponent();

	}
}
