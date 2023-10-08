// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelRuntimeProvider.h"
#include "VoxelParameterOverrideCollection_DEPRECATED.h"
#include "VoxelComponent.generated.h"

class FVoxelRuntime;
class UVoxelGraphInterface;
class UVoxelParameterContainer;

UCLASS(ClassGroup = Voxel, DisplayName = "Voxel Component", HideCategories = ("Rendering", "Physics", "LOD", "Activation", "Collision", "Cooking", "AssetUserData"), meta = (BlueprintSpawnableComponent))
class VOXELGRAPHCORE_API UVoxelComponent
	: public USceneComponent
	, public IVoxelRuntimeProvider
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Voxel", meta = (ProviderClass = "/Script/VoxelGraphCore.VoxelGraphInterface"))
	TObjectPtr<UVoxelParameterContainer> ParameterContainer;

	FSimpleMulticastDelegate OnRuntimeCreated;
	FSimpleMulticastDelegate OnRuntimeDestroyed;

public:
	UPROPERTY()
	TSoftObjectPtr<UVoxelGraphInterface> Graph_DEPRECATED;

	UPROPERTY()
	FVoxelParameterOverrideCollection_DEPRECATED ParameterCollection_DEPRECATED;

public:
	UVoxelComponent();
	virtual ~UVoxelComponent() override;

	//~ Begin USceneComponent Interface
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void PostLoad() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif
	//~ End USceneComponent Interface

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
};