// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Buffer/VoxelStaticMeshBuffer.h"
#include "Point/VoxelPointHandleProvider.h"
#include "VoxelInstancedCollisionComponent.generated.h"

class FVoxelPointOverrideChunk;

DECLARE_VOXEL_COUNTER(VOXELSPAWNER_API, STAT_VoxelNumCollisionInstances, "Num Collision Instances");

struct VOXELSPAWNER_API FVoxelInstancedCollisionDataImpl
{
	TVoxelArray<int32> FreeIndices;
	TVoxelMap<FVoxelPointId, int32> PointIdToIndex;

	TVoxelArray<int32> InstanceBodiesToUpdate;
	TVoxelArray<TSharedPtr<FBodyInstance>> InstanceBodiesToDelete;

	TVoxelArray<FVoxelPointId> PointIds;
	TVoxelArray<FTransform3f> Transforms;
	TVoxelArray<TSharedPtr<FBodyInstance>> InstanceBodies;

	FVoxelFastCriticalSection CriticalSection;
	TVoxelArray<TWeakPtr<FBodyInstance>> AllBodyInstances_RequiresLock;

	void FlushInstanceBodiesToDelete();
};

class VOXELSPAWNER_API FVoxelInstancedCollisionData
{
public:
	const FVoxelStaticMesh Mesh;
	const FVoxelPointChunkRef ChunkRef;

	FVoxelInstancedCollisionData(
		const FVoxelStaticMesh& Mesh,
		const FVoxelPointChunkRef& ChunkRef)
		: Mesh(Mesh)
		, ChunkRef(ChunkRef)
	{
	}

	FVoxelFastCriticalSection CriticalSection;

	FORCEINLINE FVoxelInstancedCollisionDataImpl& GetDataImpl_RequiresLock()
	{
		checkVoxelSlow(CriticalSection.IsLocked());
		return DataImpl;
	}

private:
	FVoxelInstancedCollisionDataImpl DataImpl;
};

UCLASS()
class VOXELSPAWNER_API UVoxelInstancedCollisionComponent
	: public UPrimitiveComponent
	, public IVoxelPointHandleProvider
{
	GENERATED_BODY()

public:
	UVoxelInstancedCollisionComponent();

	//~ Begin UPrimitiveComponent Interface
	virtual UBodySetup* GetBodySetup() override;
	virtual bool ShouldCreatePhysicsState() const override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual void OnCreatePhysicsState() override;
	virtual void OnDestroyPhysicsState() override;
	//~ End UPrimitiveComponent Interface

	//~ Begin IVoxelPointHandleProvider Interface
	virtual bool TryGetPointHandle(
		int32 ItemIndex,
		FVoxelPointHandle& OutHandle) const override;
	//~ End IVoxelPointHandleProvider Interface

	void SetData(const TSharedRef<FVoxelInstancedCollisionData>& NewData);
	void ReturnToPool();
	void Update();

	TSharedRef<FBodyInstance> MakeBodyInstance(int32 Index) const;

	FORCEINLINE TSharedPtr<FVoxelInstancedCollisionData> GetData() const
	{
		return Data;
	}

private:
	TSharedPtr<FVoxelInstancedCollisionData> Data;
	TSharedPtr<FVoxelPointOverrideChunk> OverrideChunk;
	FSharedVoidPtr OverrideChunkDelegatePtr;

	VOXEL_COUNTER_HELPER(STAT_VoxelNumCollisionInstances, NumInstances);

	friend class FVoxelInstancedCollisionSceneProxy;
};