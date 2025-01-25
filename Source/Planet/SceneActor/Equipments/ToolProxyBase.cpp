
#include "ToolProxyBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "PlanetGameplayAbility.h"
#include "ToolFuture_Base.h"

FName AToolProxyBase::RootComponentName = TEXT("RootComponent");

AToolProxyBase::AToolProxyBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SceneCompPtr = CreateDefaultSubobject<USceneComponent>(ThisClass::RootComponentName);

	SceneCompPtr->SetShouldUpdatePhysicsVolume(true);
	SceneCompPtr->SetCanEverAffectNavigation(false);
	RootComponent = SceneCompPtr;

}

void AToolProxyBase::AttachToCharacter(ACharacterBase* CharacterPtr)
{
}

void AToolProxyBase::BeginPlay()
{
	Super::BeginPlay();
}
