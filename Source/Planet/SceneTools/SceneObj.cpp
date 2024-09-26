// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SceneObj.h"

#include "Kismet/GameplayStatics.h"

USceneObjPropertyComponent::USceneObjPropertyComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

FName USceneObjPropertyComponent::ComponentName = TEXT("SceneObjPropertyComponent");

ASceneObj::ASceneObj(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	NetPriority = 3.0f;
	NetUpdateFrequency = 100.f;

	SetReplicates(true);
	SetReplicatingMovement(true);

	PropertyComponentPtr = CreateDefaultSubobject<USceneObjPropertyComponent>(USceneObjPropertyComponent::ComponentName);
}

UNetConnection* ASceneObj::GetNetConnection() const
{
	auto Controller = UGameplayStatics::GetPlayerController(GWorld, 0);
	if (Controller)
	{
		return Controller->GetNetConnection();
	}
	return Super::GetNetConnection();
}

void ASceneObj::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		auto Controller = UGameplayStatics::GetPlayerController(GWorld, 0);
		if (Controller)
		{
			SetOwner(Controller);
			// SetAutonomousProxy(true);
		}
	}
#endif

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		SetReplicates(true);
	}
#endif
}

void ASceneObj::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASceneObj::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASceneObj::Interaction(ACharacterBase* CharacterPtr)
{

}

void ASceneObj::EndLookAt()
{

}

void ASceneObj::StartLookAt(ACharacterBase* CharacterPtr)
{

}
