
#include "ToolUnitBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "PlanetGameplayAbility.h"
#include "ToolFuture_Base.h"

AToolUnitBase::AToolUnitBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SceneCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	SceneCompPtr->SetShouldUpdatePhysicsVolume(true);
	SceneCompPtr->SetCanEverAffectNavigation(false);
	RootComponent = SceneCompPtr;

}

void AToolUnitBase::AttachToCharacter(ACharacterBase* CharacterPtr)
{
}

void AToolUnitBase::BeginPlay()
{
	Super::BeginPlay();

	auto OwnerPtr = GetOwner();
	if (OwnerPtr->IsA(AController::StaticClass()))
	{
		auto CharacterPtr = Cast<AController>(OwnerPtr)->GetPawn<ACharacterBase>();

		AttachToCharacter(CharacterPtr);
	}
	else if (OwnerPtr->IsA(ACharacterBase::StaticClass()))
	{
		auto CharacterPtr = Cast<ACharacterBase>(OwnerPtr);

		AttachToCharacter(CharacterPtr);
	}
}
