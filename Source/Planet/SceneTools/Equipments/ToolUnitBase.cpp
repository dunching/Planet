
#include "ToolUnitBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "PlanetGameplayAbility.h"
#include "ToolFuture_Base.h"

FName AToolUnitBase::RootComponentName = TEXT("RootComponent");

AToolUnitBase::AToolUnitBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SceneCompPtr = CreateDefaultSubobject<USceneComponent>(ThisClass::RootComponentName);

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
}
