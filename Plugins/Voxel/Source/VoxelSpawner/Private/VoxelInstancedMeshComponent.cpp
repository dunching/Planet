// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelInstancedMeshComponent.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelInstancedMeshDataMemory);
DEFINE_VOXEL_COUNTER(STAT_VoxelInstancedMeshNumInstances);

void FVoxelInstancedMeshData::Build()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(CustomDatas_Transient.Num() == NumCustomDatas);
	ensure(PerInstanceSMData.Num() == 0);
	ensure(PerInstanceSMCustomData.Num() == 0);

	ensure(PointIds_Transient.Num() == Transforms.Num());
	PointIds_Transient.Empty();

	FVoxelUtilities::SetNumFast(PerInstanceSMData, Num());
	for (int32 Index = 0; Index < Num(); Index++)
	{
		PerInstanceSMData[Index].Transform = FMatrix(Transforms[Index].ToMatrixWithScale());
	}

	FVoxelUtilities::SetNumFast(PerInstanceSMCustomData, Num() * CustomDatas_Transient.Num());
	for (int32 CustomDataIndex = 0; CustomDataIndex < CustomDatas_Transient.Num(); CustomDataIndex++)
	{
		const TVoxelArray<float>& CustomData = CustomDatas_Transient[CustomDataIndex];
		for (int32 Index = 0; Index < Num(); Index++)
		{
			PerInstanceSMCustomData[CustomDatas_Transient.Num() * Index + CustomDataIndex] = CustomData[Index];
		}
	}

	CustomDatas_Transient.Empty();

	const FBox MeshBounds = Mesh.GetMeshInfo().MeshBox;
	if (MeshBounds.IsValid)
	{
		VOXEL_SCOPE_COUNTER("Compute bounds");

		const FVoxelBox VoxelMeshBounds(MeshBounds);

		FVoxelBox Result = VoxelMeshBounds.TransformBy(Transforms[0]);
		for (int32 Index = 1; Index < Num(); Index++)
		{
			Result += VoxelMeshBounds.TransformBy(Transforms[Index]);
		}
		Bounds = Result;
	}


	UpdateStats();
}

int64 FVoxelInstancedMeshData::GetAllocatedSize() const
{
	return
		FVoxelMeshDataBase::GetAllocatedSize() +
		PerInstanceSMData.GetAllocatedSize() +
		PerInstanceSMCustomData.GetAllocatedSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UVoxelInstancedMeshComponent::UVoxelInstancedMeshComponent()
{
	BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UVoxelInstancedMeshComponent::AddMeshData(const TSharedRef<FVoxelInstancedMeshData>& NewMeshData)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(NewMeshData->Num() > 0);

	if (!ensure(NewMeshData->InstanceIndices.Num() == NewMeshData->Num()) ||
		!ensure(NewMeshData->PerInstanceSMData.Num() == NewMeshData->Num()))
	{
		return;
	}

	if (!ensure(MeshData) ||
		!ensure(MeshData->Mesh == NewMeshData->Mesh) ||
		!ensure(MeshData->NumCustomDatas == NewMeshData->NumCustomDatas))
	{
		return;
	}

	PerInstanceSMData.AddUninitialized(NewMeshData->NumNewInstances);
	PerInstanceSMCustomData.AddUninitialized(NewMeshData->NumNewInstances * NumCustomDataFloats);

	InstanceUpdateCmdBuffer.NumAdds += NewMeshData->NumNewInstances;

	MeshData->Transforms.AddUninitialized(NewMeshData->NumNewInstances);

	for (int32 Index = 0; Index < NewMeshData->Num(); Index++)
	{
		const int32 InstanceIndex = NewMeshData->InstanceIndices[Index];
		if (!ensure(PerInstanceSMData.IsValidIndex(InstanceIndex)))
		{
			continue;
		}

		MeshData->Transforms[InstanceIndex] = NewMeshData->Transforms[Index];

		const FInstancedStaticMeshInstanceData InstanceData = NewMeshData->PerInstanceSMData[Index];

		ToCompatibleVoxelArray(PerInstanceSMData)[InstanceIndex] = InstanceData;

		for (int32 CustomFloatIndex = 0; CustomFloatIndex < NumCustomDataFloats; CustomFloatIndex++)
		{
			const int32 CustomIndex = NumCustomDataFloats * Index + CustomFloatIndex;
			const int32 InstanceCustomIndex = NumCustomDataFloats * InstanceIndex + CustomFloatIndex;
			ToCompatibleVoxelArray(PerInstanceSMCustomData)[InstanceCustomIndex] = NewMeshData->PerInstanceSMCustomData[CustomIndex];
		}
	}

	MeshData->UpdateStats();
	NumInstances = MeshData->Num();

	// Edit will trigger a full render buffer recreate when combined with MarkRenderStateDirty
	InstanceUpdateCmdBuffer.NumEdits++;
	MarkRenderStateDirty();
}

void UVoxelInstancedMeshComponent::SetMeshData(const TSharedRef<FVoxelInstancedMeshData>& NewMeshData)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(NewMeshData->Num() > 0);

	ensure(NewMeshData->NumNewInstances == NewMeshData->Num());
	NewMeshData->InstanceIndices.Empty();

	MeshData = NewMeshData;

	SetStaticMesh(NewMeshData->Mesh.StaticMesh.Get());

	PerInstanceSMData = MoveTemp(MeshData->PerInstanceSMData);
	NumCustomDataFloats = MeshData->NumCustomDatas;
	PerInstanceSMCustomData = MoveTemp(MeshData->PerInstanceSMCustomData);

	// Edit will trigger a full render buffer recreate when combined with MarkRenderStateDirty
	InstanceUpdateCmdBuffer.NumEdits++;
	MarkRenderStateDirty();

	MeshData->UpdateStats();
	NumInstances = MeshData->Num();
}

void UVoxelInstancedMeshComponent::RemoveInstancesFast(const TConstVoxelArrayView<int32> Indices)
{
	HideInstances(Indices);
}

void UVoxelInstancedMeshComponent::ReturnToPool()
{
	ClearInstances();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelInstancedMeshComponent::HideInstances(const TConstVoxelArrayView<int32> Indices)
{
	VOXEL_FUNCTION_COUNTER();

	const FMatrix EmptyMatrix = FTransform(FQuat::Identity, FVector::ZeroVector, FVector::ZeroVector).ToMatrixWithScale();

	for (const int32 Index : Indices)
	{
		if (!ensure(PerInstanceSMData.IsValidIndex(Index)))
		{
			continue;
		}

		PerInstanceSMData[Index].Transform = EmptyMatrix;
	}

	// Edit will trigger a full render buffer recreate when combined with MarkRenderStateDirty
	InstanceUpdateCmdBuffer.NumEdits++;
	MarkRenderStateDirty();
}

void UVoxelInstancedMeshComponent::ShowInstances(const TConstVoxelArrayView<int32> Indices)
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

	// Edit will trigger a full render buffer recreate when combined with MarkRenderStateDirty
	InstanceUpdateCmdBuffer.NumEdits++;
	MarkRenderStateDirty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelInstancedMeshComponent::ClearInstances()
{
	VOXEL_FUNCTION_COUNTER();

	NumInstances = 0;
	MeshData.Reset();

	InstanceStartCullDistance = GetDefault<UVoxelInstancedMeshComponent>()->InstanceStartCullDistance;
	InstanceEndCullDistance = GetDefault<UVoxelInstancedMeshComponent>()->InstanceEndCullDistance;

	AsyncVoxelTask([MeshDataPtr = MakeUniqueCopy(MeshData)]
	{
		VOXEL_SCOPE_COUNTER("Delete MeshData");
		MeshDataPtr->Reset();
	});

	MeshData.Reset();

	Super::ClearInstances();
}

void UVoxelInstancedMeshComponent::DestroyComponent(const bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

	MeshData.Reset();
	NumInstances = 0;
}