// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelDefaultPointActor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AVoxelDefaultPointActor::AVoxelDefaultPointActor()
{
	bReplicates = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	RootComponent = MeshComponent;
}

void AVoxelDefaultPointActor::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->SetStaticMesh(Mesh);
}

void AVoxelDefaultPointActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVoxelDefaultPointActor, Mesh);
}