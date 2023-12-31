// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SceneObj.h"

USceneObjPropertyComponent::USceneObjPropertyComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

FName USceneObjPropertyComponent::ComponentName = TEXT("SceneObjPropertyComponent");

ASceneObj::ASceneObj(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
	PropertyComponentPtr = CreateDefaultSubobject<USceneObjPropertyComponent>(USceneObjPropertyComponent::ComponentName);
}

void ASceneObj::BeginPlay()
{
	Super::BeginPlay();
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