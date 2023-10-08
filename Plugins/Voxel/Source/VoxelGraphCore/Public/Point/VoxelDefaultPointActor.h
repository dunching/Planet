// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Point/VoxelPointActor.h"
#include "VoxelDefaultPointActor.generated.h"

UCLASS()
class VOXELGRAPHCORE_API AVoxelDefaultPointActor
	: public AActor
	, public IVoxelPointActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Voxel", Replicated)
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Voxel")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	AVoxelDefaultPointActor();

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor Interface
};