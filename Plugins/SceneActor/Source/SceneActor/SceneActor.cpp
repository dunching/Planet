// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SceneActor.h"

#include "Kismet/GameplayStatics.h"

USceneActorPropertyComponent::USceneActorPropertyComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

FName USceneActorPropertyComponent::ComponentName = TEXT("SceneObjPropertyComponent");

ASceneActor::ASceneActor(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;
	SetNetUpdateFrequency(100.f);

	SetReplicatingMovement(true);

	PropertyComponentPtr = CreateDefaultSubobject<USceneActorPropertyComponent>(USceneActorPropertyComponent::ComponentName);
}

UNetConnection* ASceneActor::GetNetConnection() const
{
	// auto Controller = UGameplayStatics::GetPlayerController(GWorld, 0);
	// if (Controller)
	// {
	// 	return Controller->GetNetConnection();
	// }
	return Super::GetNetConnection();
}

void ASceneActor::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		auto Controller = UGameplayStatics::GetPlayerController(GWorld, 0);
		if (Controller)
		{
			// SetOwner(Controller);
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

void ASceneActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASceneActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
