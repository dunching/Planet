// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelRenderMeshChunk.h"
#include "VoxelRuntime.h"
#include "VoxelNodeMessages.h"
#include "VoxelBufferUtilities.h"
#include "VoxelFoliageSettings.h"
#include "VoxelInstancedMeshComponent.h"
#include "VoxelHierarchicalMeshComponent.h"
#include "Point/VoxelPointOverrideManager.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELSPAWNER_API, int32, GVoxelMaxInstancesToAddAtOnce, 16,
	"voxel.MaxInstancesToAddAtOnce",
	"");

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelRenderMeshChunk);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelRenderMeshChunk::FVoxelRenderMeshChunk(
	const FVoxelGraphNodeRef& NodeRef,
	const FVoxelPointChunkRef& ChunkRef,
	const TSharedRef<const FVoxelRuntimeInfo>& RuntimeInfo,
	const TVoxelDynamicValue<FVoxelPointSet>& PointsValue,
	const float RenderDistance,
	const float MinRenderDistance,
	const float FadeDistance,
	const TSharedRef<const FVoxelFoliageSettings>& FoliageSettings)
	: NodeRef(NodeRef)
	, ChunkRef(ChunkRef)
	, RuntimeInfo(RuntimeInfo)
	, PointsValue(PointsValue)
	, RenderDistance(RenderDistance)
	, MinRenderDistance(MinRenderDistance)
	, FadeDistance(FadeDistance)
	, FoliageSettings(FoliageSettings)
	, OverrideChunk(FVoxelPointOverrideManager::Get(RuntimeInfo->GetWorld())->FindOrAddChunk(ChunkRef))
{
}

void FVoxelRenderMeshChunk::Initialize()
{
	PointsValue.OnChanged(MakeWeakPtrLambda(this, [this](const TSharedRef<const FVoxelPointSet>& NewPoints)
	{
		UpdatePoints(NewPoints);
	}));

	VOXEL_SCOPE_LOCK(OverrideChunk->CriticalSection);
	OverrideChunk->OnChanged_RequiresLock.Add(MakeWeakPtrDelegate(this, [this](const TConstVoxelArrayView<FVoxelPointId> PointIds)
	{
		VOXEL_SCOPE_COUNTER("OnChanged");
		VOXEL_SCOPE_LOCK(CriticalSection);
		UpdatePointOverrides_AssumeLocked(PointIds);
	}));
}

void FVoxelRenderMeshChunk::Destroy(FVoxelRuntime& Runtime)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);
	check(IsInGameThread());

	for (const auto& It : MeshToComponents_RequiresLock)
	{
		Runtime.DestroyComponent(It.Value->InstancedMeshComponent);
		Runtime.DestroyComponent(It.Value->HierarchicalMeshComponent);
	}
}

FVoxelOptionalBox FVoxelRenderMeshChunk::GetBounds() const
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);
	ensure(IsInGameThread());

	FVoxelOptionalBox Result;
	for (const auto& It : MeshToComponents_RequiresLock)
	{
		const UVoxelHierarchicalMeshComponent* Component = It.Value->HierarchicalMeshComponent.Get();
		if (!Component)
		{
			continue;
		}

		const TSharedPtr<const FVoxelHierarchicalMeshData> MeshData = Component->GetMeshData();
		if (!ensure(MeshData))
		{
			continue;
		}

		ensure(MeshData->Bounds.IsValid());
		Result += MeshData->Bounds;
	}
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelRenderMeshChunk::UpdatePoints(const TSharedRef<const FVoxelPointSet>& NewPoints)
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelRenderMeshChunk::UpdatePoints Num=%d", NewPoints->Num());
	ensure(!IsInGameThread());
	FVoxelNodeStatScope StatScope(*this, NewPoints->Num());

	FindVoxelPointSetAttributeVoid(*NewPoints, FVoxelPointAttributes::Id, FVoxelPointIdBuffer, NewIds);
	FindVoxelPointSetAttributeVoid(*NewPoints, FVoxelPointAttributes::Mesh, FVoxelStaticMeshBuffer, NewMeshes);
	FindVoxelPointSetAttributeVoid(*NewPoints, FVoxelPointAttributes::Position, FVoxelVectorBuffer, NewPositions);
	FindVoxelPointSetOptionalAttribute(*NewPoints, FVoxelPointAttributes::Rotation, FVoxelQuaternionBuffer, NewRotations, FQuat::Identity);
	FindVoxelPointSetOptionalAttribute(*NewPoints, FVoxelPointAttributes::Scale, FVoxelVectorBuffer, NewScales, FVector::OneVector);
	const TVoxelArray<FVoxelFloatBuffer> NewCustomDatas = NewPoints->FindCustomDatas(GetNodeRef());

	TVoxelAddOnlyMap<FVoxelPointId, int32> NewPointIdToIndex;
	{
		VOXEL_SCOPE_COUNTER("PointIdToIndex");

		NewPointIdToIndex.Reserve(NewPoints->Num());

		for (int32 Index = 0; Index < NewPoints->Num(); Index++)
		{
			const FVoxelPointId PointId = NewIds[Index];
			if (NewPointIdToIndex.Contains(PointId))
			{
				VOXEL_MESSAGE(Error, "{0}: PointIds duplicates!", this);
				continue;
			}

			NewPointIdToIndex.Add_CheckNew_NoRehash(PointId, Index);
		}
	}

	VOXEL_SCOPE_LOCK(CriticalSection);

	ON_SCOPE_EXIT
	{
		PointIdToIndex_RequiresLock = MoveTemp(NewPointIdToIndex);

		const TSharedRef<FVoxelPointSet> CleanNewPoints = MakeVoxelShared<FVoxelPointSet>();
		if (NewPoints->Num() > 0)
		{
			CleanNewPoints->SetNum(NewPoints->Num());

			for (const auto& It : NewPoints->GetAttributes())
			{
				if (It.Key == FVoxelPointAttributes::Id ||
					It.Key == FVoxelPointAttributes::Mesh ||
					It.Key == FVoxelPointAttributes::Position ||
					It.Key == FVoxelPointAttributes::Rotation ||
					It.Key == FVoxelPointAttributes::Scale ||
					It.Key.GetComparisonIndex() == FVoxelPointAttributes::CustomData.GetComparisonIndex())
				{
					CleanNewPoints->Add(It.Key, It.Value.ToSharedRef());
				}
			}
		}
		Points_RequiresLock = CleanNewPoints;
	};

	if (!Points_RequiresLock)
	{
		UpdatePoints_Hierarchical_AssumeLocked(NewPoints);
		return;
	}

	FindVoxelPointSetAttributeVoid(*Points_RequiresLock, FVoxelPointAttributes::Id, FVoxelPointIdBuffer, OldIds);
	FindVoxelPointSetAttributeVoid(*Points_RequiresLock, FVoxelPointAttributes::Mesh, FVoxelStaticMeshBuffer, OldMeshes);
	FindVoxelPointSetAttributeVoid(*Points_RequiresLock, FVoxelPointAttributes::Position, FVoxelVectorBuffer, OldPositions);
	FindVoxelPointSetOptionalAttribute(*Points_RequiresLock, FVoxelPointAttributes::Rotation, FVoxelQuaternionBuffer, OldRotations, FQuat::Identity);
	FindVoxelPointSetOptionalAttribute(*Points_RequiresLock, FVoxelPointAttributes::Scale, FVoxelVectorBuffer, OldScales, FVector::OneVector);
	const TVoxelArray<FVoxelFloatBuffer> OldCustomDatas = Points_RequiresLock->FindCustomDatas(GetNodeRef());

	if (NewCustomDatas.Num() != OldCustomDatas.Num())
	{
		UpdatePoints_Hierarchical_AssumeLocked(NewPoints);
		return;
	}
	const int32 NumCustomDatas = NewCustomDatas.Num();

	TVoxelArray<int32> NewToOldIndex;
	FVoxelUtilities::SetNumFast(NewToOldIndex, NewPointIdToIndex.Num());
	{
		VOXEL_SCOPE_COUNTER("NewToOldIndex");

		for (const auto& It : NewPointIdToIndex)
		{
			if (const int32* IndexPtr = PointIdToIndex_RequiresLock.Find(It.Key))
			{
				NewToOldIndex[It.Value] = *IndexPtr;
			}
			else
			{
				NewToOldIndex[It.Value] = -1;
			}
		}
	}

	int32 NumNewInstances = 0;
	TVoxelChunkedArray<FVoxelPointId> PointIdsToRemove;
	TVoxelAddOnlyMap<FVoxelStaticMesh, TSharedPtr<FVoxelInstancedMeshData>> MeshToMeshData;
	{
		VOXEL_SCOPE_COUNTER("MeshToMeshData")

		for (int32 NewIndex = 0; NewIndex < NewToOldIndex.Num(); NewIndex++)
		{
			const FVoxelStaticMesh Mesh = NewMeshes[NewIndex];
			const FVector3f Position = NewPositions[NewIndex];
			const FQuat4f Rotation = NewRotations[NewIndex];
			const FVector3f Scale = NewScales[NewIndex];

			const int32 OldIndex = NewToOldIndex[NewIndex];
			if (OldIndex != -1)
			{
				checkVoxelSlow(OldIds[OldIndex] == NewIds[NewIndex]);

				const FVoxelStaticMesh OldMesh = OldMeshes[OldIndex];
				const FVector3f OldPosition = OldPositions[OldIndex];
				const FQuat4f OldRotation = OldRotations[OldIndex];
				const FVector3f OldScale = OldScales[OldIndex];

				if (OldMesh != Mesh)
				{
					goto DifferentData;
				}

				if (OldPosition != Position ||
					OldRotation != Rotation ||
					OldScale != Scale)
				{
					goto DifferentData;
				}

				for (int32 Index = 0; Index < NumCustomDatas; Index++)
				{
					if (OldCustomDatas[Index][OldIndex] != NewCustomDatas[Index][NewIndex])
					{
						goto DifferentData;
					}
				}

				continue;

			DifferentData:
				PointIdsToRemove.Add(OldIds[OldIndex]);
			}

			NumNewInstances++;

			if (NumNewInstances > GVoxelMaxInstancesToAddAtOnce)
			{
				UpdatePoints_Hierarchical_AssumeLocked(NewPoints);
				return;
			}

			const FVoxelPointId PointId = NewIds[NewIndex];

			TSharedPtr<FVoxelInstancedMeshData>& MeshData = MeshToMeshData.FindOrAdd(Mesh);
			if (!MeshData)
			{
				MeshData = MakeVoxelShared<FVoxelInstancedMeshData>(Mesh, ChunkRef);
				MeshData->NumCustomDatas = NumCustomDatas;
				MeshData->CustomDatas_Transient.SetNum(NumCustomDatas);
			}

			MeshData->PointIds_Transient.Add(PointId);
			MeshData->Transforms.Add(FTransform3f(Rotation, Position, Scale));

			for (int32 Index = 0; Index < NumCustomDatas; Index++)
			{
				MeshData->CustomDatas_Transient[Index].Add(NewCustomDatas[Index][NewIndex]);
			}
		}
	}

	TVoxelAddOnlyMap<FVoxelStaticMesh, TVoxelArray<int32>> MeshToInstancedInstancesToRemove;
	TVoxelAddOnlyMap<FVoxelStaticMesh, TVoxelArray<int32>> MeshToHierarchicalInstancesToRemove;
	{
		VOXEL_SCOPE_COUNTER("PointIdsToRemove");

		for (const auto& It : PointIdToIndex_RequiresLock)
		{
			if (!NewPointIdToIndex.Contains(It.Key))
			{
				PointIdsToRemove.Add(It.Key);
			}
		}

		for (const FVoxelPointId PointId : PointIdsToRemove)
		{
			const int32* OldIndexPtr = PointIdToIndex_RequiresLock.Find(PointId);
			if (!ensure(OldIndexPtr))
			{
				continue;
			}

			const FVoxelStaticMesh Mesh = OldMeshes[*OldIndexPtr];
			const TSharedPtr<FComponents>* ComponentsPtr = MeshToComponents_RequiresLock.Find(Mesh);
			if (!ensure(ComponentsPtr))
			{
				continue;
			}

			FIndexInfo* IndexInfo = (**ComponentsPtr).PointIdToIndexInfo.Find(PointId);
			if (!ensure(IndexInfo) ||
				!ensure(IndexInfo->bIsValid))
			{
				continue;
			}

			if (!IndexInfo->bIsHierarchical)
			{
				MeshToInstancedInstancesToRemove.FindOrAdd(Mesh).Add(IndexInfo->Index);
			}
			else
			{
				MeshToHierarchicalInstancesToRemove.FindOrAdd(Mesh).Add(IndexInfo->Index);
			}

			IndexInfo->Raw = -1;
			IndexInfo->bIsValid = false;
		}
	}

	for (const auto& It : MeshToMeshData)
	{
		VOXEL_SCOPE_COUNTER("PointIdToIndexInfo");

		TSharedPtr<FComponents>& ComponentsPtr = MeshToComponents_RequiresLock.FindOrAdd(It.Key);
		if (!ComponentsPtr)
		{
			ComponentsPtr = MakeVoxelShared<FComponents>();
		}

		FComponents& Components = *ComponentsPtr;
		FVoxelInstancedMeshData& MeshData = *It.Value;
		TVoxelArray<int32>& InstancedInstancesToRemove = MeshToInstancedInstancesToRemove.FindOrAdd(It.Key);

		MeshData.InstanceIndices.Reserve(MeshData.Num());
		Components.PointIdToIndexInfo.Reserve(Components.PointIdToIndexInfo.Num() + MeshData.Num());

		for (const FVoxelPointId PointId : MeshData.PointIds_Transient)
		{
			FIndexInfo& IndexInfo = Components.PointIdToIndexInfo.FindOrAdd(PointId);
			if (!ensureVoxelSlow(!IndexInfo.bIsValid))
			{
				continue;
			}

			int32 Index;
			if (InstancedInstancesToRemove.Num() > 0)
			{
				// Steal an index we're going to remove
				Index = InstancedInstancesToRemove.Pop(false);
			}
			else if (Components.FreeInstancedIndices.Num() > 0)
			{
				Index = Components.FreeInstancedIndices.Pop(false);
			}
			else
			{
				MeshData.NumNewInstances++;
				Index = Components.NumInstancedInstances++;
			}
			MeshData.InstanceIndices.Add(Index);

			IndexInfo.bIsValid = true;
			IndexInfo.bIsHierarchical = false;
			IndexInfo.Index = Index;
		}

		Components.FreeInstancedIndices.Append(InstancedInstancesToRemove);
	}

	for (auto& It : MeshToMeshData)
	{
		It.Value->Build();
	}

	FVoxelUtilities::RunOnGameThread_Async(MakeWeakPtrLambda(this, [
		this,
		MeshToMeshData = MoveTemp(MeshToMeshData),
		MeshToInstancedInstancesToRemove = MoveTemp(MeshToInstancedInstancesToRemove),
		MeshToHierarchicalInstancesToRemove = MoveTemp(MeshToHierarchicalInstancesToRemove)]
	{
		VOXEL_SCOPE_COUNTER("UpdatePoints_GameThread");
		VOXEL_SCOPE_LOCK(CriticalSection);

		const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
		if (!ensure(Runtime))
		{
			return;
		}

		// Make sure to process instances to remove BEFORE setting new mesh data,
		// as it might reuse the same indices

		for (const auto& It : MeshToInstancedInstancesToRemove)
		{
			if (It.Value.Num() == 0)
			{
				// Indices were reused or we added it through the FindOrAdd above
				continue;
			}

			const TSharedPtr<FComponents> Components = MeshToComponents_RequiresLock.FindRef(It.Key);
			if (!ensure(Components))
			{
				continue;
			}

			UVoxelInstancedMeshComponent* Component = Components->InstancedMeshComponent.Get();
			if (!ensure(Component))
			{
				continue;
			}

			Component->RemoveInstancesFast(It.Value);
		}

		for (const auto& It : MeshToHierarchicalInstancesToRemove)
		{
			const TSharedPtr<FComponents> Components = MeshToComponents_RequiresLock.FindRef(It.Key);
			if (!ensure(Components))
			{
				continue;
			}

			UVoxelHierarchicalMeshComponent* Component = Components->HierarchicalMeshComponent.Get();
			if (!ensure(Component))
			{
				continue;
			}

			Component->RemoveInstancesFast(It.Value);
		}

		for (const auto& It : MeshToMeshData)
		{
			const TSharedPtr<FComponents> Components = MeshToComponents_RequiresLock.FindRef(It.Key);
			if (!ensure(Components))
			{
				continue;
			}

			UVoxelInstancedMeshComponent* Component = Components->InstancedMeshComponent.Get();
			if (!Component)
			{
				Component = Runtime->CreateComponent<UVoxelInstancedMeshComponent>();
				Components->InstancedMeshComponent = Component;
			}
			if (!ensure(Component))
			{
				continue;
			}

			if (!Component->HasMeshData())
			{
				Component->SetMeshData(It.Value.ToSharedRef());
				FoliageSettings->ApplyToComponent(*Component);
			}
			else
			{
				Component->AddMeshData(It.Value.ToSharedRef());
			}

			Component->InstanceStartCullDistance = RenderDistance - FadeDistance;
			Component->InstanceEndCullDistance = RenderDistance;
		}
	}));

	// CriticalSection will be locked when PointIdsToHide_RequiresLock will be accessed
	UpdatePointOverrides_AssumeLocked(OverrideChunk->PointIdsToHide_RequiresLock);
}

void FVoxelRenderMeshChunk::UpdatePoints_Hierarchical_AssumeLocked(const TSharedRef<const FVoxelPointSet>& Points)
{
	VOXEL_FUNCTION_COUNTER();
	check(CriticalSection.IsLocked());

	// Clear existing data
	for (auto& It : MeshToComponents_RequiresLock)
	{
		It.Value->PointIdToIndexInfo.Reset();
		It.Value->FreeInstancedIndices.Reset();
		It.Value->NumInstancedInstances = 0;
	}

	if (Points->Num() == 0)
	{
		FVoxelUtilities::RunOnGameThread_Async(MakeWeakPtrLambda(this, [this]
		{
			SetHierarchicalDatas_GameThread({});
		}));
		return;
	}

	FindVoxelPointSetAttributeVoid(*Points, FVoxelPointAttributes::Mesh, FVoxelStaticMeshBuffer, Meshes);

	FVoxelInt32Buffer MeshIndices;
	FVoxelStaticMeshBuffer MeshPalette;
	FVoxelBufferUtilities::MakePalette(Meshes, MeshIndices, MeshPalette);

	for (const FVoxelStaticMesh& Mesh : MeshPalette)
	{
		if (!Mesh.StaticMesh.IsValid())
		{
			VOXEL_MESSAGE(Error, "{0}: Mesh is null", this);
		}
	}

	TVoxelArray<int32> MeshIndexToNumInstances;
	MeshIndexToNumInstances.SetNum(MeshPalette.Num());
	{
		VOXEL_SCOPE_COUNTER("MeshIndexToNumInstances");

		for (int32 Index = 0; Index < Points->Num(); Index++)
		{
			MeshIndexToNumInstances[MeshIndices[Index]]++;
		}
	}

	TVoxelArray<TSharedPtr<FVoxelHierarchicalMeshData>> HierarchicalMeshDatas;
	for (const FVoxelStaticMesh& Mesh : MeshPalette)
	{
		HierarchicalMeshDatas.Add(MakeVoxelShared<FVoxelHierarchicalMeshData>(Mesh, ChunkRef));
	}

	{
		VOXEL_SCOPE_COUNTER("PointIds & Transforms");

		FindVoxelPointSetAttributeVoid(*Points, FVoxelPointAttributes::Id, FVoxelPointIdBuffer, Ids);

		for (int32 MeshIndex = 0; MeshIndex < HierarchicalMeshDatas.Num(); MeshIndex++)
		{
			FVoxelHierarchicalMeshData& MeshData = *HierarchicalMeshDatas[MeshIndex];
			const int32 NumInstances = MeshIndexToNumInstances[MeshIndex];

			MeshData.PointIds_Transient.Reserve(NumInstances);
			MeshData.Transforms.Reserve(NumInstances);
		}

		FindVoxelPointSetAttributeVoid(*Points, FVoxelPointAttributes::Position, FVoxelVectorBuffer, Positions);
		FindVoxelPointSetOptionalAttribute(*Points, FVoxelPointAttributes::Rotation, FVoxelQuaternionBuffer, Rotations, FQuat::Identity);
		FindVoxelPointSetOptionalAttribute(*Points, FVoxelPointAttributes::Scale, FVoxelVectorBuffer, Scales, FVector::OneVector);

		for (int32 Index = 0; Index < Points->Num(); Index++)
		{
			FVoxelHierarchicalMeshData& MeshData = *HierarchicalMeshDatas[MeshIndices[Index]].Get();
			MeshData.PointIds_Transient.Add_NoGrow(Ids[Index]);
			MeshData.Transforms.Add_NoGrow(FTransform3f(Rotations[Index], Positions[Index], Scales[Index]));
		}
	}

	{
		VOXEL_SCOPE_COUNTER("CustomDatas");

		const TVoxelArray<FVoxelFloatBuffer> CustomDatas = Points->FindCustomDatas(GetNodeRef());

		for (int32 MeshIndex = 0; MeshIndex < HierarchicalMeshDatas.Num(); MeshIndex++)
		{
			FVoxelHierarchicalMeshData& MeshData = *HierarchicalMeshDatas[MeshIndex];
			const int32 NumInstances = MeshIndexToNumInstances[MeshIndex];

			MeshData.NumCustomDatas = CustomDatas.Num();
			MeshData.CustomDatas_Transient.SetNum(CustomDatas.Num());
			for (TVoxelArray<float>& CustomData : MeshData.CustomDatas_Transient)
			{
				CustomData.Reserve(NumInstances);
			}
		}

		for (int32 CustomDataIndex = 0; CustomDataIndex < CustomDatas.Num(); CustomDataIndex++)
		{
			const FVoxelFloatBuffer& CustomDataBuffer = CustomDatas[CustomDataIndex];

			for (int32 Index = 0; Index < Points->Num(); Index++)
			{
				FVoxelHierarchicalMeshData& MeshData = *HierarchicalMeshDatas[MeshIndices[Index]].Get();
				MeshData.CustomDatas_Transient[CustomDataIndex].Add_NoGrow(CustomDataBuffer[Index]);
			}
		}
	}

	for (const TSharedPtr<FVoxelHierarchicalMeshData>& HierarchicalMeshData : HierarchicalMeshDatas)
	{
		VOXEL_SCOPE_COUNTER("PointIdToIndexInfo");

		TSharedPtr<FComponents>& ComponentsPtr = MeshToComponents_RequiresLock.FindOrAdd(HierarchicalMeshData->Mesh);
		if (!ComponentsPtr)
		{
			ComponentsPtr = MakeVoxelShared<FComponents>();
		}
		FComponents& Components = *ComponentsPtr;

		ensure(Components.PointIdToIndexInfo.Num() == 0);
		Components.PointIdToIndexInfo.Reserve(HierarchicalMeshData->Num());

		const TVoxelArray<FVoxelPointId>& PointIds = HierarchicalMeshData->PointIds_Transient;
		for (int32 Index = 0; Index < PointIds.Num(); Index++)
		{
			const FVoxelPointId PointId = PointIds[Index];
			FIndexInfo& IndexInfo = Components.PointIdToIndexInfo.FindOrAdd(PointId);
			if (!ensureVoxelSlow(!IndexInfo.bIsValid))
			{
				continue;
			}

			IndexInfo.bIsValid = true;
			IndexInfo.bIsHierarchical = true;
			IndexInfo.Index = Index;
		}
	}

	for (const TSharedPtr<FVoxelHierarchicalMeshData>& HierarchicalMeshData : HierarchicalMeshDatas)
	{
		HierarchicalMeshData->Build();
	}

	FVoxelUtilities::RunOnGameThread_Async(MakeWeakPtrLambda(this, [this, HierarchicalMeshDatas]
	{
		SetHierarchicalDatas_GameThread(HierarchicalMeshDatas);
	}));

	// CriticalSection will be locked when PointIdsToHide_RequiresLock will be accessed
	UpdatePointOverrides_AssumeLocked(OverrideChunk->PointIdsToHide_RequiresLock);
}

void FVoxelRenderMeshChunk::SetHierarchicalDatas_GameThread(
	const TVoxelArray<TSharedPtr<FVoxelHierarchicalMeshData>>& NewHierarchicalMeshDatas)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsInGameThread());

	const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
	if (!ensure(Runtime))
	{
		return;
	}

	VOXEL_SCOPE_LOCK(CriticalSection);

	// Clear existing meshes
	for (const auto& It : MeshToComponents_RequiresLock)
	{
		ensure(It.Value->FreeInstancedIndices.Num() == 0);
		ensure(It.Value->NumInstancedInstances == 0);

		if (UVoxelInstancedMeshComponent* Component = It.Value->InstancedMeshComponent.Get())
		{
			Component->ClearInstances();
		}
		if (UVoxelHierarchicalMeshComponent* Component = It.Value->HierarchicalMeshComponent.Get())
		{
			Component->ClearInstances();
		}
	}

	for (const TSharedPtr<FVoxelHierarchicalMeshData>& HierarchicalData : NewHierarchicalMeshDatas)
	{
		const FVoxelStaticMesh Mesh = HierarchicalData->Mesh;
		if (!Mesh.StaticMesh.IsValid())
		{
			continue;
		}

		const TSharedPtr<FComponents> Components = MeshToComponents_RequiresLock.FindRef(Mesh);
		if (!ensure(Components))
		{
			continue;
		}

		UVoxelHierarchicalMeshComponent* Component = Components->HierarchicalMeshComponent.Get();
		if (!Component)
		{
			Component = Runtime->CreateComponent<UVoxelHierarchicalMeshComponent>();
			Components->HierarchicalMeshComponent = Component;
		}
		if (!ensure(Component))
		{
			continue;
		}

		Component->SetMeshData(HierarchicalData.ToSharedRef());
		FoliageSettings->ApplyToComponent(*Component);

		Component->InstanceStartCullDistance = RenderDistance - FadeDistance;
		Component->InstanceEndCullDistance = RenderDistance;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename PointIdsType>
void FVoxelRenderMeshChunk::UpdatePointOverrides_AssumeLocked(const PointIdsType& PointIdsToUpdate)
{
	VOXEL_FUNCTION_COUNTER();
	checkVoxelSlow(CriticalSection.IsLocked());
	VOXEL_SCOPE_LOCK(OverrideChunk->CriticalSection);

	for (const auto& It : MeshToComponents_RequiresLock)
	{
		TVoxelArray<int32> InstancedInstancesToHide;
		TVoxelArray<int32> InstancedInstancesToShow;

		TVoxelArray<int32> HierarchicalInstancesToHide;
		TVoxelArray<int32> HierarchicalInstancesToShow;

		for (const FVoxelPointId& PointId : PointIdsToUpdate)
		{
			const FIndexInfo* IndexInfo = It.Value->PointIdToIndexInfo.Find(PointId);
			if (!IndexInfo ||
				!IndexInfo->bIsValid)
			{
				continue;
			}

			const bool bShouldHide = OverrideChunk->PointIdsToHide_RequiresLock.Contains(PointId);

			if (IndexInfo->bIsHierarchical)
			{
				if (bShouldHide)
				{
					HierarchicalInstancesToHide.Add(IndexInfo->Index);
				}
				else
				{
					HierarchicalInstancesToShow.Add(IndexInfo->Index);
				}
			}
			else
			{
				if (bShouldHide)
				{
					InstancedInstancesToHide.Add(IndexInfo->Index);
				}
				else
				{
					InstancedInstancesToShow.Add(IndexInfo->Index);
				}
			}
		}

		if (InstancedInstancesToHide.Num() == 0 &&
			InstancedInstancesToShow.Num() == 0 &&
			HierarchicalInstancesToHide.Num() == 0 &&
			HierarchicalInstancesToShow.Num() == 0)
		{
			continue;
		}

		// _Async to ensure scheduling with other game thread tasks is correct
		FVoxelUtilities::RunOnGameThread_Async(MakeWeakPtrLambda(this, [=, Components = It.Value]
		{
			if (UVoxelInstancedMeshComponent* Component = Components->InstancedMeshComponent.Get())
			{
				if (InstancedInstancesToHide.Num() > 0)
				{
					Component->HideInstances(InstancedInstancesToHide);
				}
				if (InstancedInstancesToShow.Num() > 0)
				{
					Component->ShowInstances(InstancedInstancesToShow);
				}
			}

			if (UVoxelHierarchicalMeshComponent* Component = Components->HierarchicalMeshComponent.Get())
			{
				if (HierarchicalInstancesToHide.Num() > 0)
				{
					Component->HideInstances(HierarchicalInstancesToHide);
				}
				if (HierarchicalInstancesToShow.Num() > 0)
				{
					Component->ShowInstances(HierarchicalInstancesToShow);
				}
			}
		}));
	}
}