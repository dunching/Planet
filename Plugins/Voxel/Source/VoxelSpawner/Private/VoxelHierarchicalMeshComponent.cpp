// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelHierarchicalMeshComponent.h"
#include "VoxelInstancedMeshComponent.h"
#include "Point/VoxelPointOverrideManager.h"
#include "ShowFlags.h"
#include "UnrealClient.h"
#include "Engine/InstancedStaticMesh.h"
#if WITH_EDITOR
#include "EditorViewportClient.h"
#endif

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelHierarchicalMeshDataMemory);
DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelHierarchicalMeshMemory);
DEFINE_VOXEL_COUNTER(STAT_VoxelHierarchicalMeshNumInstances);

void FVoxelHierarchicalMeshData::Build()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(!BuiltData);
	ensure(CustomDatas_Transient.Num() == NumCustomDatas);

	ensure(PointIds_Transient.Num() == Transforms.Num());
	PointIds_Transient.Empty();

	const TSharedRef<FVoxelHierarchicalMeshBuiltData> NewBuiltData = MakeVoxelShared<FVoxelHierarchicalMeshBuiltData>();
	UVoxelHierarchicalMeshComponent::AsyncTreeBuild(
		*NewBuiltData,
		Mesh.GetMeshInfo().MeshBox,
		Mesh.GetMeshInfo().DesiredInstancesPerLeaf,
		*this);

	if (ensure(NewBuiltData->ClusterTree.Num() > 0))
	{
		ensure(!Bounds.IsValid());
		Bounds = FVoxelBox(
			FVector(NewBuiltData->ClusterTree[0].BoundMin),
			FVector(NewBuiltData->ClusterTree[0].BoundMax));
	}

	CustomDatas_Transient.Empty();

	ensure(!BuiltData);
	BuiltData = NewBuiltData;

	UpdateStats();
}

int64 FVoxelHierarchicalMeshData::GetAllocatedSize() const
{
	int64 AllocatedSize = FVoxelMeshDataBase::GetAllocatedSize();
	if (BuiltData)
	{
		AllocatedSize += BuiltData->InstanceDatas.GetAllocatedSize();
		AllocatedSize += BuiltData->CustomDatas.GetAllocatedSize();
		AllocatedSize += BuiltData->InstanceReorderTable.GetAllocatedSize();
	}
	return AllocatedSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
class FVoxelHierarchicalMeshComponentTicker : public FVoxelTicker
{
public:
	//~ Begin FVoxelTicker Interface
	virtual void Tick() override
	{
		VOXEL_FUNCTION_COUNTER();

		if (!ensure(GIsEditor))
		{
			return;
		}

		TVoxelAddOnlyMap<UWorld*, bool> WorldToShouldHide;
		const auto ShouldHide = [&](UWorld* World)
		{
			if (WorldToShouldHide.Contains(World))
			{
				return WorldToShouldHide[World];
			}

			bool bShouldHide = false;

			// Iterate over all viewports, to detect ejected viewports
			for (const FEditorViewportClient* ViewportClient : GEditor->GetAllViewportClients())
			{
				if (ViewportClient->GetWorld() != World)
				{
					continue;
				}

				bShouldHide |=
					ViewportClient->EngineShowFlags.Collision ||
					ViewportClient->EngineShowFlags.CollisionPawn ||
					ViewportClient->EngineShowFlags.CollisionVisibility;
			}

			WorldToShouldHide.Add_CheckNew(World, bShouldHide);
			return bShouldHide;
		};

		ForEachObjectOfClass<UVoxelHierarchicalMeshComponent>([&](UVoxelHierarchicalMeshComponent* Component)
		{
			UWorld* World = Component->GetWorld();
			if (!World)
			{
				return;
			}

			const bool bShouldBeVisible = !ShouldHide(World);

			if (Component->GetVisibleFlag() != bShouldBeVisible)
			{
				Component->SetVisibleFlag(bShouldBeVisible);
				Component->MarkRenderStateDirty();
			}
		});

		ForEachObjectOfClass<UVoxelInstancedMeshComponent>([&](UVoxelInstancedMeshComponent* Component)
		{
			UWorld* World = Component->GetWorld();
			if (!World)
			{
				return;
			}

			const bool bShouldBeVisible = !ShouldHide(World);

			if (Component->GetVisibleFlag() != bShouldBeVisible)
			{
				Component->SetVisibleFlag(bShouldBeVisible);
				Component->MarkRenderStateDirty();
			}
		});
	}
	//~ End FVoxelTicker Interface
};

VOXEL_RUN_ON_STARTUP_EDITOR(MakeVoxelHierarchicalMeshComponentTicker)
{
	check(GIsEditor);
	new FVoxelHierarchicalMeshComponentTicker();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UVoxelHierarchicalMeshComponent::UVoxelHierarchicalMeshComponent()
{
	bDisableCollision = true;
	BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UVoxelHierarchicalMeshComponent::SetMeshData(const TSharedRef<const FVoxelHierarchicalMeshData>& NewMeshData)
{
	ClearInstances();

	ensure(!MeshData);
	MeshData = NewMeshData;

	SetStaticMesh(NewMeshData->Mesh.StaticMesh.Get());

	MeshData->UpdateStats();

	if (ensure(MeshData->BuiltData))
	{
		SetBuiltData(MoveTemp(*MeshData->BuiltData));
		MeshData->BuiltData.Reset();
	}

#if WITH_EDITOR
	FVoxelSystemUtilities::EnsureViewportIsUpToDate();
#endif

	MeshData->UpdateStats();
	NumInstances = MeshData->Num();

	UpdateStats();
}

void UVoxelHierarchicalMeshComponent::RemoveInstancesFast(const TConstVoxelArrayView<int32> Indices)
{
	HideInstances(Indices);
}

void UVoxelHierarchicalMeshComponent::ReturnToPool()
{
	ClearInstances();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelHierarchicalMeshComponent::HideInstances(const TConstVoxelArrayView<int32> Indices)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(MeshData))
	{
		return;
	}

	const FMatrix EmptyMatrix = FTransform(FQuat::Identity, FVector::ZeroVector, FVector::ZeroVector).ToMatrixWithScale();
	const FMatrix44f EmptyMatrixFloat = FMatrix44f(EmptyMatrix);

	if (PerInstanceSMData.Num() > 0)
	{
		for (const int32 Index : Indices)
		{
			if (!ensure(PerInstanceSMData.IsValidIndex(Index)))
			{
				continue;
			}

			PerInstanceSMData[Index].Transform = EmptyMatrix;
		}
	}

	if (!ensure(PerInstanceRenderData))
	{
		return;
	}

	const TSharedPtr<FStaticMeshInstanceData> InstanceBuffer = PerInstanceRenderData->InstanceBuffer_GameThread;
	if (!ensure(InstanceBuffer))
	{
		return;
	}

	for (const int32 Index : Indices)
	{
		if (!ensure(InstanceReorderTable.IsValidIndex(Index)))
		{
			continue;
		}

		const int32 BuiltIndex = InstanceReorderTable[Index];
		if (!ensure(0 <= BuiltIndex && BuiltIndex < InstanceBuffer->GetNumInstances()))
		{
			continue;
		}

		InstanceBuffer->SetInstance(BuiltIndex, EmptyMatrixFloat, 0);
	}

	MarkRenderStateDirty();
}

void UVoxelHierarchicalMeshComponent::ShowInstances(const TConstVoxelArrayView<int32> Indices)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(MeshData))
	{
		return;
	}

	if (PerInstanceSMData.Num() > 0)
	{
		for (const int32 Index : Indices)
		{
			if (!ensure(PerInstanceSMData.IsValidIndex(Index)) ||
				!ensure(MeshData->Transforms.IsValidIndex(Index)))
			{
				continue;
			}

			PerInstanceSMData[Index].Transform = FMatrix(MeshData->Transforms[Index].ToMatrixWithScale());
		}
	}

	if (!ensure(PerInstanceRenderData))
	{
		return;
	}

	const TSharedPtr<FStaticMeshInstanceData> InstanceBuffer = PerInstanceRenderData->InstanceBuffer_GameThread;
	if (!ensure(InstanceBuffer))
	{
		return;
	}

	for (const int32 Index : Indices)
	{
		if (!ensure(InstanceReorderTable.IsValidIndex(Index)) ||
			!ensure(MeshData->Transforms.IsValidIndex(Index)))
		{
			continue;
		}

		const int32 BuiltIndex = InstanceReorderTable[Index];
		if (!ensure(0 <= BuiltIndex && BuiltIndex < InstanceBuffer->GetNumInstances()))
		{
			continue;
		}

		InstanceBuffer->SetInstance(BuiltIndex, MeshData->Transforms[Index].ToMatrixWithScale(), 0);
	}

	MarkRenderStateDirty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int64 UVoxelHierarchicalMeshComponent::GetAllocatedSize() const
{
	int64 AllocatedSize = 0;
	if (ClusterTreePtr)
	{
		AllocatedSize += ClusterTreePtr->GetAllocatedSize();
	}
	if (PerInstanceRenderData &&
		PerInstanceRenderData->InstanceBuffer_GameThread)
	{
		AllocatedSize += PerInstanceRenderData->InstanceBuffer_GameThread->GetResourceSize();
	}
	AllocatedSize += PerInstanceSMData.GetAllocatedSize();
	AllocatedSize += PerInstanceSMCustomData.GetAllocatedSize();
	AllocatedSize += InstanceReorderTable.GetAllocatedSize();
	return AllocatedSize;
}

void UVoxelHierarchicalMeshComponent::ReleasePerInstanceRenderData_Safe()
{
	VOXEL_FUNCTION_COUNTER();

	if (!PerInstanceRenderData.IsValid())
	{
		return;
	}

	// Ensure all UpdateBoundsTransforms_Concurrent tasks are completed
	VOXEL_ENQUEUE_RENDER_COMMAND(FlushPerInstanceRenderDataBounds)([PerInstanceRenderData = PerInstanceRenderData](FRHICommandListImmediate& RHICmdList)
	{
		struct FPerInstanceRenderDataAlias
		{
			TArray<TRefCountPtr<HHitProxy>> HitProxies;
			SIZE_T ResourceSize;
			FStaticMeshInstanceBuffer InstanceBuffer;
			TSharedPtr<FStaticMeshInstanceData, ESPMode::ThreadSafe> InstanceBuffer_GameThread;
			TArray<FVector4f> PerInstanceBounds;
			TArray<FRenderTransform> PerInstanceTransforms;
			FGraphEventRef UpdateBoundsTask;
			FBox InstanceLocalBounds;
			bool bTrackBounds;
			bool bBoundsTransformsDirty;
		};

		const FGraphEventRef& Task = ReinterpretCastRef<FPerInstanceRenderDataAlias>(*PerInstanceRenderData).UpdateBoundsTask;
		if (Task.IsValid())
		{
			VOXEL_SCOPE_COUNTER("Wait for UpdateBoundsTransforms");
			Task->Wait();
		}
	});

	ReleasePerInstanceRenderData();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelHierarchicalMeshComponent::ClearInstances()
{
	VOXEL_FUNCTION_COUNTER();

	NumInstances = 0;

	InstanceStartCullDistance = GetDefault<UVoxelHierarchicalMeshComponent>()->InstanceStartCullDistance;
	InstanceEndCullDistance = GetDefault<UVoxelHierarchicalMeshComponent>()->InstanceEndCullDistance;

	AsyncVoxelTask([MeshDataPtr = MakeUniqueCopy(MeshData)]
	{
		VOXEL_SCOPE_COUNTER("Delete MeshData");
		MeshDataPtr->Reset();
	});

	MeshData.Reset();

	ReleasePerInstanceRenderData_Safe();

	Super::ClearInstances();

	UpdateStats();
}

void UVoxelHierarchicalMeshComponent::DestroyComponent(const bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

	MeshData.Reset();
	NumInstances = 0;
	UpdateStats();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelHierarchicalMeshComponent::AsyncTreeBuild(
	FVoxelHierarchicalMeshBuiltData& OutBuiltData,
	const FBox& MeshBox,
	const int32 InDesiredInstancesPerLeaf,
	const FVoxelHierarchicalMeshData& InMeshData)
{
	VOXEL_SCOPE_COUNTER_FORMAT("UVoxelHierarchicalMeshComponent::AsyncTreeBuild Num=%d", InMeshData.Transforms.Num());

	const int32 NumInstances = InMeshData.Transforms.Num();
	check(NumInstances > 0);

	OutBuiltData.InstanceBuffer = MakeUnique<FStaticMeshInstanceData>(false);
	{
		VOXEL_SCOPE_COUNTER("AllocateInstances");
		OutBuiltData.InstanceBuffer->AllocateInstances(
			NumInstances,
			InMeshData.CustomDatas_Transient.Num(),
			EResizeBufferFlags::None,
			true);
	}

	TCompatibleVoxelArray<FMatrix> Matrices;
	FVoxelUtilities::SetNumFast(Matrices, NumInstances);
	{
		VOXEL_SCOPE_COUNTER("SetInstances");
		for (int32 Index = 0; Index < NumInstances; Index++)
		{
			FTransform3f Transform = InMeshData.Transforms[Index];
			Transform.NormalizeRotation();
			const FMatrix44f Matrix = Transform.ToMatrixWithScale();

			const uint64 Seed = FVoxelUtilities::MurmurHash(Transform.GetTranslation());
			const FRandomStream RandomStream(Seed);

			Matrices[Index] = FMatrix(Matrix);
			OutBuiltData.InstanceBuffer->SetInstance(Index, Matrix, RandomStream.GetFraction());
		}
	}

	OutBuiltData.InstanceDatas = ReinterpretCastVoxelArray<FInstancedStaticMeshInstanceData>(Matrices);

	FVoxelUtilities::SetNumFast(OutBuiltData.CustomDatas, InMeshData.CustomDatas_Transient.Num() * NumInstances);

	{
		VOXEL_SCOPE_COUNTER("Set Custom Data");
		for (int32 CustomDataIndex = 0; CustomDataIndex < InMeshData.CustomDatas_Transient.Num(); CustomDataIndex++)
		{
			const TVoxelArray<float>& CustomData = InMeshData.CustomDatas_Transient[CustomDataIndex];
			if (!ensure(CustomData.Num() == NumInstances))
			{
				continue;
			}

			for (int32 InstanceIndex = 0; InstanceIndex < NumInstances; InstanceIndex++)
			{
				OutBuiltData.CustomDatas[InstanceIndex * InMeshData.CustomDatas_Transient.Num() + CustomDataIndex] = CustomData[InstanceIndex];
				OutBuiltData.InstanceBuffer->SetInstanceCustomData(InstanceIndex, CustomDataIndex, CustomData[InstanceIndex]);
			}
		}
	}

	TCompatibleVoxelArray<int32> SortedInstances;
	TCompatibleVoxelArray<int32> InstanceReorderTable;
	{
		VOXEL_SCOPE_COUNTER("BuildTreeAnyThread");
		TArray<float> CustomDataFloats;
		// TODO
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		BuildTreeAnyThread(
			Matrices,
			// Done manually
			CustomDataFloats,
			0,
			MeshBox,
			OutBuiltData.ClusterTree,
			SortedInstances,
			InstanceReorderTable,
			OutBuiltData.OcclusionLayerNum,
			FMath::Max(InDesiredInstancesPerLeaf, 1),
			false
		);
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}

	OutBuiltData.InstanceReorderTable = InstanceReorderTable;

	// In-place sort the instances
	{
		VOXEL_SCOPE_COUNTER("Sort Instances");
		for (int32 FirstUnfixedIndex = 0; FirstUnfixedIndex < NumInstances; FirstUnfixedIndex++)
		{
			const int32 LoadFrom = SortedInstances[FirstUnfixedIndex];
			if (LoadFrom == FirstUnfixedIndex)
			{
				continue;
			}

			check(LoadFrom > FirstUnfixedIndex);
			OutBuiltData.InstanceBuffer->SwapInstance(FirstUnfixedIndex, LoadFrom);
			const int32 SwapGoesTo = InstanceReorderTable[FirstUnfixedIndex];
			checkVoxelSlow(SwapGoesTo > FirstUnfixedIndex);
			checkVoxelSlow(SortedInstances[SwapGoesTo] == FirstUnfixedIndex);
			SortedInstances[SwapGoesTo] = LoadFrom;
			InstanceReorderTable[LoadFrom] = SwapGoesTo;
			InstanceReorderTable[FirstUnfixedIndex] = FirstUnfixedIndex;
			SortedInstances[FirstUnfixedIndex] = FirstUnfixedIndex;
		}
	}
}

void UVoxelHierarchicalMeshComponent::SetBuiltData(FVoxelHierarchicalMeshBuiltData&& BuiltData)
{
	VOXEL_FUNCTION_COUNTER();

	if (PerInstanceRenderData.IsValid())
	{
		ReleasePerInstanceRenderData_Safe();
	}

	const int32 NewNumInstances = BuiltData.InstanceBuffer->GetNumInstances();

	{
		VOXEL_SCOPE_COUNTER("InitPerInstanceRenderData");
		constexpr bool bRequireCPUAccess = false;
		InitPerInstanceRenderData(true, BuiltData.InstanceBuffer.Get(), bRequireCPUAccess);
	}

	{
		VOXEL_SCOPE_COUNTER("AcceptPrebuiltTree");
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		AcceptPrebuiltTree(BuiltData.InstanceDatas, BuiltData.ClusterTree, BuiltData.OcclusionLayerNum, NewNumInstances);
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}

	// AcceptPrebuiltTree does not move data for non nanite proxies
	if (!ShouldCreateNaniteProxy())
	{
		ensure(PerInstanceSMData.Num() == 0);
		PerInstanceSMData = MoveTemp(BuiltData.InstanceDatas);
	}
	else
	{
		ensure(PerInstanceSMData.Num() == NewNumInstances);
	}

	NumCustomDataFloats = MeshData->NumCustomDatas;
	PerInstanceSMCustomData = MoveTemp(BuiltData.CustomDatas);
	InstanceReorderTable = MoveTemp(BuiltData.InstanceReorderTable);
}