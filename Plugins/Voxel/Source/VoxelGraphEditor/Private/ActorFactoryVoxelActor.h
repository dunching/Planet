// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "ActorFactoryVoxelActor.generated.h"

UCLASS()
class UActorFactoryVoxelActor : public UActorFactory
{
	GENERATED_BODY()

public:
	UActorFactoryVoxelActor();
};

UCLASS()
class UActorFactoryVoxelActor_FromGraph : public UActorFactory
{
	GENERATED_BODY()

public:
	UActorFactoryVoxelActor_FromGraph();

	//~ Begin UActorFactory Interface
	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	virtual UObject* GetAssetFromActorInstance(AActor* ActorInstance) override;
	//~ End UActorFactory Interface
};