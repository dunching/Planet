// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelRuntimeProvider.h"
#include "VoxelRuntimeParameter.h"
#include "VoxelParameterOverrideCollection_DEPRECATED.h"
#include "VoxelActor.generated.h"

class FVoxelRuntime;
class UVoxelGraphInterface;
class UVoxelParameterContainer;
class UVoxelPointStorage;
class UVoxelSculptStorage;

UCLASS()
class VOXELGRAPHCORE_API UVoxelActorRootComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	//~ Begin UPrimitiveComponent Interface
	virtual bool MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* Hit, EMoveComponentFlags MoveFlags, ETeleportType Teleport) override;
	//~ End UPrimitiveComponent Interface
};

UCLASS(HideCategories = ("Rendering", "Replication", "Input", "Collision", "LOD", "HLOD", "Cooking", "DataLayers", "Networking", "Physics"))
class VOXELGRAPHCORE_API AVoxelActor
	: public AActor
	, public IVoxelRuntimeProvider
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Voxel", meta = (ProviderClass = "/Script/VoxelGraphCore.VoxelGraphInterface"))
	TObjectPtr<UVoxelParameterContainer> ParameterContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Voxel")
	bool bCreateRuntimeOnBeginPlay = true;

#if WITH_EDITOR
	bool bCreateRuntimeOnConstruction_EditorOnly = true;
#endif

	FSimpleMulticastDelegate OnRuntimeCreated;
	FSimpleMulticastDelegate OnRuntimeDestroyed;

public:
	UPROPERTY()
	TObjectPtr<UVoxelPointStorage> PointStorageComponent;

	UPROPERTY()
	TObjectPtr<UVoxelSculptStorage> SculptStorageComponent;

public:
	UPROPERTY()
	TSoftObjectPtr<UVoxelGraphInterface> Graph_DEPRECATED;

	UPROPERTY()
	FVoxelParameterOverrideCollection_DEPRECATED ParameterCollection_DEPRECATED;

public:
	AVoxelActor();
	virtual ~AVoxelActor() override;

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Destroyed() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostLoad() override;
	virtual void PostEditImport() override;

#if WITH_EDITOR
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;
	virtual void PreEditChange(FProperty* PropertyThatWillChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
	//~ End AActor Interface

	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

public:
	FVoxelRuntimeParameters DefaultRuntimeParameters;

	virtual FVoxelRuntimeParameters GetRuntimeParameters() const;

public:
	//~ Begin IVoxelRuntimeProvider Interface
	virtual TSharedPtr<FVoxelRuntime> GetRuntime() const final override
	{
		return Runtime;
	}
	//~ End IVoxelRuntimeProvider Interface

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	bool IsRuntimeCreated() const
	{
		return Runtime.IsValid();
	}

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void QueueRecreate()
	{
		bRuntimeRecreateQueued = true;
	}

	// Will call CreateRuntime when it's ready to be created
	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void QueueCreateRuntime();

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void CreateRuntime();

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void DestroyRuntime();

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	UVoxelGraphInterface* GetGraph() const;

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void SetGraph(UVoxelGraphInterface* NewGraph);

private:
	bool bRuntimeCreateQueued = false;
	bool bRuntimeRecreateQueued = false;
	TSharedPtr<FVoxelRuntime> Runtime;

private:
	bool bDisableModify = false;

	friend FVoxelRuntime;
};