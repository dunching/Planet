#include "HumanCharacter_AI.h"

#include <Kismet/GameplayStatics.h>
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"

#include "AIComponent.h"
#include "CharacterTitle.h"
#include "CharacterBase.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter.h"
#include "InventoryComponent.h"
#include "GameplayTagsLibrary.h"
#include "SceneProxyExtendInfo.h"
#include "CharactersInfo.h"
#include "HumanAIController.h"
#include "GroupSharedInfo.h"
#include "AIControllerStateTreeAIComponent.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"

// UGameplayTasksComponent* USceneCharacterAIInteractionComponent::GetGameplayTasksComponent(
// 	const UGameplayTask& Task) const
// {
// 	return GetOwner<AHumanCharacter_AI>()->GetCharacterAbilitySystemComponent();
// }

void UCharacterAIAttributesComponent::SetCharacterID(const FGuid& InCharacterID)
{
	Super::SetCharacterID(InCharacterID);
	
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OwnerCharacterPtr = GetOwner<AHumanCharacter_AI>();
	}
#endif
}

AHumanCharacter_AI::AHumanCharacter_AI(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	AIComponentPtr = CreateDefaultSubobject<UAIComponent>(UAIComponent::ComponentName);
	
	InteractionWidgetCompoentPtr = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	InteractionWidgetCompoentPtr->SetupAttachment(RootComponent);
}

void AHumanCharacter_AI::BeginPlay()
{
	Super::BeginPlay();
}

void AHumanCharacter_AI::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AHumanCharacter_AI::HasBeenStartedLookAt(ACharacterBase* InCharacterPtr)
{
	Super::HasBeenStartedLookAt(InCharacterPtr);
	
	HasBeenLookingAt(InCharacterPtr);
}

void AHumanCharacter_AI::HasBeenLookingAt(ACharacterBase* InCharacterPtr)
{
	Super::HasBeenLookingAt(InCharacterPtr);
	
	if (
		InteractionWidgetCompoentPtr &&
		(FVector::Distance(InCharacterPtr->GetActorLocation(), GetActorLocation()) < SceneActorInteractionComponentPtr->Range)
		)
	{
		InteractionWidgetCompoentPtr->SetVisibility(true);
	}
	else
	{
		HasBeenEndedLookAt();
	}
	
}

void AHumanCharacter_AI::HasBeenEndedLookAt()
{	
	if (InteractionWidgetCompoentPtr)
	{
		InteractionWidgetCompoentPtr->SetVisibility(false);
	}
	
	Super::HasBeenEndedLookAt();
}

void AHumanCharacter_AI::SetGroupSharedInfo(AGroupSharedInfo* InGroupSharedInfoPtr)
{
	GroupSharedInfoPtr = InGroupSharedInfoPtr;

	if (auto ControllerPtr = GetController<AHumanAIController>())
	{
		ControllerPtr->SetGroupSharedInfo(InGroupSharedInfoPtr);
	}
}

void AHumanCharacter_AI::SetCharacterID(const FGuid& InCharacterID)
{
	GetCharacterAttributesComponent()->SetCharacterID(InCharacterID);
}

UAIComponent* AHumanCharacter_AI::GetAIComponent() const
{
	return  AIComponentPtr;
}

void AHumanCharacter_AI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AHumanCharacter_AI::OnRep_GroupSharedInfoChanged()
{
	Super::OnRep_GroupSharedInfoChanged();
}

// TSharedPtr<FCharacterProxy> AHumanCharacter_AI::GetCharacterProxy() const
// {
// 	return GetGroupSharedInfo()->GetInventoryComponent()->FindProxy_Character(CharacterID);
// }

void AHumanCharacter_AI::OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr)
{
	Super::OnGroupSharedInfoReady(NewGroupSharedInfoPtr);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		if (GetParentActor())
		{
		}
		// 如果这个Character是单独的，则直接生成 
		else
		{
			GetGroupSharedInfo()->GetTeamMatesHelperComponent()->OwnerCharacterProxyPtr = GetCharacterProxy();
		}
	}
#endif
}
