
#include "PlanetAIController.h"

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
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "TalentAllocationComponent.h"
#include "SceneUnitContainer.h"

APlanetAIController::APlanetAIController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	CharacterAttributesComponentPtr = CreateDefaultSubobject<UCharacterAttributesComponent>(UCharacterAttributesComponent::ComponentName);
	HoldingItemsComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
	TalentAllocationComponentPtr = CreateDefaultSubobject<UTalentAllocationComponent>(UTalentAllocationComponent::ComponentName);
	GroupMnaggerComponentPtr = CreateDefaultSubobject<UGroupMnaggerComponent>(UGroupMnaggerComponent::ComponentName);
}

void APlanetAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	BindPCWithCharacter();
}

UPlanetAbilitySystemComponent* APlanetAIController::GetAbilitySystemComponent() const
{
	return GetPawn<FPawnType>()->GetAbilitySystemComponent();
}

UGroupMnaggerComponent* APlanetAIController::GetGroupMnaggerComponent() const
{
	return GroupMnaggerComponentPtr;
}

UHoldingItemsComponent* APlanetAIController::GetHoldingItemsComponent() const
{
	return HoldingItemsComponentPtr;
}

UCharacterAttributesComponent* APlanetAIController::GetCharacterAttributesComponent() const
{
	return CharacterAttributesComponentPtr;
}

UTalentAllocationComponent* APlanetAIController::GetTalentAllocationComponent() const
{
	return TalentAllocationComponentPtr;
}

UCharacterUnit* APlanetAIController::GetGourpMateUnit()
{
	return CharacterUnitPtr;
}

ACharacterBase* APlanetAIController::GetRealCharacter()const
{
	return Cast<ACharacterBase>(GetPawn());
}

void APlanetAIController::BeginPlay()
{
	Super::BeginPlay();
}

void APlanetAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CharacterUnitPtr->RelieveRootBind();
	CharacterUnitPtr = nullptr;

	Super::EndPlay(EndPlayReason);
}

void APlanetAIController::ResetGroupmateUnit(UCharacterUnit* NewGourpMateUnitPtr)
{
	CharacterUnitPtr = NewGourpMateUnitPtr;

	CharacterUnitPtr->ProxyCharacterPtr = GetPawn<FPawnType>();
	CharacterUnitPtr->CharacterAttributes->Name = GetCharacterAttributesComponent()->GetCharacterAttributes().Name;
}

void APlanetAIController::BindPCWithCharacter()
{
	// AI的控制器，第一次控制的角色即为控制器始终绑定的角色
	if (!RealCharacter)
	{
		RealCharacter = GetPawn<FPawnType>();

		CharacterUnitPtr = nullptr;

		// 通过临时 SceneUnitContainer 的创建UnitPtr
		auto SceneUnitContainer = HoldingItemsComponentPtr->GetSceneUnitContainer();

		ResetGroupmateUnit(SceneUnitContainer->AddUnit_Groupmate(RowName));

		CharacterUnitPtr->SceneUnitContainer = SceneUnitContainer;

		SceneUnitContainer = HoldingItemsComponentPtr->GetSceneUnitContainer();

		SceneUnitContainer->AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_Regular, 0);
		SceneUnitContainer->AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_RafflePermanent, 0);
		SceneUnitContainer->AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_RaffleLimit, 0);
	}
}
