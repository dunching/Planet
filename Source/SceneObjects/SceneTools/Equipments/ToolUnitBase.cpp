
#include "ToolUnitBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

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

void AToolUnitBase::AttachToCharacter(FOnwerType* CharacterPtr)
{
}

void AToolUnitBase::BeginPlay()
{
	Super::BeginPlay();

	auto CharacterPtr = Cast<FOnwerType>(GetOwner());

	AttachToCharacter(CharacterPtr);
}
