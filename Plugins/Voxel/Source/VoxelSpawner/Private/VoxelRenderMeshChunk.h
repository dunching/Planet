// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNodeHelpers.h"
#include "Point/VoxelChunkedPointSet.h"
#include "Buffer/VoxelStaticMeshBuffer.h"

struct FVoxelFoliageSettings;
struct FVoxelHierarchicalMeshData;
class FVoxelPointOverrideChunk;
class UVoxelInstancedMeshComponent;
class UVoxelHierarchicalMeshComponent;

class FVoxelRenderMeshChunk
	: public IVoxelNodeInterface
	, public TSharedFromThis<FVoxelRenderMeshChunk>
	, public TVoxelRuntimeInfo<FVoxelRenderMeshChunk>
{
public:
	const FVoxelGraphNodeRef NodeRef;
	const FVoxelPointChunkRef ChunkRef;
	const TSharedRef<const FVoxelRuntimeInfo> RuntimeInfo;
	const TVoxelDynamicValue<FVoxelPointSet> PointsValue;
	const float RenderDistance;
	const float MinRenderDistance;
	const float FadeDistance;
	const TSharedRef<const FVoxelFoliageSettings> FoliageSettings;
	const TSharedRef<FVoxelPointOverrideChunk> OverrideChunk;

	VOXEL_COUNT_INSTANCES();

	FVoxelRenderMeshChunk(
		const FVoxelGraphNodeRef& NodeRef,
		const FVoxelPointChunkRef& ChunkRef,
		const TSharedRef<const FVoxelRuntimeInfo>& RuntimeInfo,
		const TVoxelDynamicValue<FVoxelPointSet>& PointsValue,
		float RenderDistance,
		float MinRenderDistance,
		float FadeDistance,
		const TSharedRef<const FVoxelFoliageSettings>& FoliageSettings);

	FORCEINLINE const FVoxelRuntimeInfo& GetRuntimeInfoRef() const
	{
		return *RuntimeInfo;
	}

	void Initialize();
	void Destroy(FVoxelRuntime& Runtime);
	FVoxelOptionalBox GetBounds() const;

	//~ Begin IVoxelNodeInterface Interface
	virtual const FVoxelGraphNodeRef& GetNodeRef() const override
	{
		return NodeRef;
	}
	//~ End IVoxelNodeInterface Interface

private:
	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelAddOnlyMap<FVoxelPointId, int32> PointIdToIndex_RequiresLock;
	TSharedPtr<const FVoxelPointSet> Points_RequiresLock;

	struct FIndexInfo
	{
		union
		{
			struct
			{
				uint32 Index : 30;
				uint32 bIsValid : 1;
				uint32 bIsHierarchical : 1;
			};
			uint32 Raw = 0;
		};
	};
	checkStatic(sizeof(FIndexInfo) == sizeof(uint32));
	struct FComponents
	{
		TVoxelAddOnlyMap<FVoxelPointId, FIndexInfo> PointIdToIndexInfo;
		TVoxelArray<int32> FreeInstancedIndices;
		int32 NumInstancedInstances = 0;

		TWeakObjectPtr<UVoxelInstancedMeshComponent> InstancedMeshComponent;
		TWeakObjectPtr<UVoxelHierarchicalMeshComponent> HierarchicalMeshComponent;
	};
	TVoxelMap<FVoxelStaticMesh, TSharedPtr<FComponents>> MeshToComponents_RequiresLock;

	void UpdatePoints(const TSharedRef<const FVoxelPointSet>& NewPoints);
	void UpdatePoints_Hierarchical_AssumeLocked(const TSharedRef<const FVoxelPointSet>& Points);
	void SetHierarchicalDatas_GameThread(const TVoxelArray<TSharedPtr<FVoxelHierarchicalMeshData>>& NewHierarchicalMeshDatas);

	template<typename PointIdsType>
	void UpdatePointOverrides_AssumeLocked(const PointIdsType& PointIdsToUpdate);
};