#include "GroupManagger.h"

#include "HumanCharacter.h"
#include "TeamMatesHelperComponent.h"

#include "InventoryComponent.h"
#include "PlanetPlayerController.h"
#include "UGSAbilitySystemComponent.h"

AGroupManagger::AGroupManagger(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer.
	       SetDefaultSubobjectClass<UTeamMatesHelperComponent>(
	                                                           UTeamMatesHelperComponent::ComponentName
	                                                          )
	       .
	       SetDefaultSubobjectClass<UInventoryComponent>(
	                                                     UInventoryComponent::ComponentName
	                                                    )
	      )
{
}

void AGroupManagger::BeginPlay()
{
	Super::BeginPlay();
}

void AGroupManagger::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		//
		GetTeamMatesHelperComponent()->SwitchTeammateOption(ETeammateOption::kInitialize);
	}
}

UPlanetAbilitySystemComponent* AGroupManagger::GetAbilitySystemComponent() const
{
	return AbilitySystemComponentPtr;
}

UTeamMatesHelperComponent* AGroupManagger::GetTeamMatesHelperComponent()
{
	return Cast<UTeamMatesHelperComponent>(TeamMatesHelperComponentPtr);
}

UInventoryComponent* AGroupManagger::GetInventoryComponent()
{
	return Cast<UInventoryComponent>(InventoryComponentPtr);
}

void AGroupManagger::SetOwnerCharacterProxyPtr(
	ACharacterBase* CharacterPtr
	)
{
	if (CharacterPtr != nullptr)
	{
		GetTeamMatesHelperComponent()->SetOwnerCharacterProxy(CharacterPtr->GetCharacterProxy());
	}
}
