#include "ToolProxyBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

FName AToolProxyBase::RootComponentName = TEXT("RootComponent");

AToolProxyBase::AToolProxyBase(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	SceneCompPtr = CreateDefaultSubobject<USceneComponent>(ThisClass::RootComponentName);

	SceneCompPtr->SetShouldUpdatePhysicsVolume(true);
	SceneCompPtr->SetCanEverAffectNavigation(false);
	RootComponent = SceneCompPtr;
}

void AToolProxyBase::AttachToCharacter(
	ACharacter* CharacterPtr
	)
{
	auto OldOwnerPtr=  GetOwner();
	if (OldOwnerPtr)
	{
		CharacterPtr->OnDestroyed.RemoveDynamic(this, &ThisClass::OnOwnerDestroyde);
	}
	
	SetOwner(CharacterPtr);
	if (CharacterPtr)
	{
		CharacterPtr->OnDestroyed.AddDynamic(this, &ThisClass::OnOwnerDestroyde);
	}
}

void AToolProxyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AToolProxyBase::OnOwnerDestroyde(
	AActor* DestroyedActor
	)
{
	Destroy();
}
