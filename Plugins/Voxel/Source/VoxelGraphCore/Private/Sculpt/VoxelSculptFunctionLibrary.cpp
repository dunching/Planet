// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Sculpt/VoxelSculptFunctionLibrary.h"
#include "Sculpt/VoxelSculptStorage.h"
#include "Sculpt/VoxelSculptStorageData.h"
#include "Sculpt/VoxelGetSculptSurfaceNode.h"
#include "Sculpt/VoxelEditSculptSurfaceExecNode.h"
#include "VoxelActor.h"
#include "VoxelRuntime.h"
#include "VoxelTaskGroup.h"
#include "VoxelDependency.h"
#include "VoxelPositionQueryParameter.h"

void UVoxelSculptFunctionLibrary::ApplySculpt(
	AVoxelActor* TargetActor,
	AVoxelActor* SculptActor)
{
	VOXEL_FUNCTION_COUNTER();

	if (!TargetActor)
	{
		VOXEL_MESSAGE(Error, "TargetActor is null");
		return;
	}
	if (!SculptActor)
	{
		VOXEL_MESSAGE(Error, "SculptActor is null");
		return;
	}

	if (!TargetActor->IsRuntimeCreated())
	{
		VOXEL_MESSAGE(Error, "{0}: TargetActor voxel runtime is not created", TargetActor);
		return;
	}
	if (!SculptActor->IsRuntimeCreated())
	{
		VOXEL_MESSAGE(Error, "{0}: SculptActor voxel runtime is not created", SculptActor);
		return;
	}

	if (!ensure(TargetActor->SculptStorageComponent))
	{
		return;
	}

	const TSharedPtr<const FVoxelRuntimeParameter_SculptStorage> SculptStorageParameter = TargetActor->DefaultRuntimeParameters.Find<FVoxelRuntimeParameter_SculptStorage>();
	if (!ensure(SculptStorageParameter))
	{
		return;
	}
	ensure(SculptStorageParameter->Data == TargetActor->SculptStorageComponent->GetData());

	const float VoxelSize = SculptStorageParameter->VoxelSize;
	const TSharedPtr<const TVoxelComputeValue<FVoxelSurface>> Compute = INLINE_LAMBDA
	{
		VOXEL_SCOPE_LOCK(SculptStorageParameter->CriticalSection);
		return SculptStorageParameter->Compute_RequiresLock;
	};

	if (!Compute)
	{
		VOXEL_MESSAGE(Error, "{0}: missing Set Sculpt Source Surface", TargetActor);
		return;
	}

	const TSharedPtr<const FVoxelRuntimeParameter_EditSculptSurface> EditSculptSurfaceParameter = SculptActor->DefaultRuntimeParameters.Find<FVoxelRuntimeParameter_EditSculptSurface>();
	if (!ensure(EditSculptSurfaceParameter))
	{
		return;
	}

	const TSharedPtr<FVoxelEditSculptSurfaceExecNodeRuntime> NodeRuntime = EditSculptSurfaceParameter->WeakRuntime.Pin();
	if (!NodeRuntime)
	{
		VOXEL_MESSAGE(Error, "{0}: No Edit Sculpt Surface node", SculptActor);
		return;
	}

	const TOptional<FVoxelBounds> OptionalLocalBounds = FVoxelTaskGroup::TryRunSynchronously(NodeRuntime->GetContext(), [&]
	{
		const FVoxelQuery Query = FVoxelQuery::Make(
			FVoxelQueryContext::Make(TargetActor->GetRuntime()->GetRuntimeInfoRef().AsShared()),
			MakeVoxelShared<FVoxelQueryParameters>(),
			FVoxelDependencyTracker::Create("VoxelSculpt"))
			.MakeNewQuery(NodeRuntime->GetContext());

		return NodeRuntime->GetNodeRuntime().Get(NodeRuntime->Node.BoundsPin, Query);
	});

	if (!ensure(OptionalLocalBounds))
	{
		return;
	}

	const FVoxelBox LocalBounds = OptionalLocalBounds->GetBox_NoDependency(
		FScaleMatrix(VoxelSize) *
		TargetActor->ActorToWorld().ToMatrixWithScale());

	// MakeMultipleOf to not have to handle partial chunk updates & querying the source data manually again
	const FVoxelIntBox IntBounds = FVoxelIntBox::FromFloatBox_WithPadding(LocalBounds)
		.MakeMultipleOfBigger(FVoxelSculptStorageData::ChunkSize);

	if (IntBounds.Count_LargeBox() > 1024 * 1024)
	{
		VOXEL_MESSAGE(Error, "Cannot apply tool: more than 1M voxels would be computed");
		return;
	}

	const TSharedRef<FVoxelQueryParameters> QueryParameters = MakeVoxelShared<FVoxelQueryParameters>();
	QueryParameters->Add<FVoxelLODQueryParameter>().LOD = 0;
	QueryParameters->Add<FVoxelPositionQueryParameter>().InitializeGrid(FVector3f(IntBounds.Min) * VoxelSize, VoxelSize, IntBounds.Size());
	{
		const TSharedRef<FVoxelSculptStorageQueryParameter> Parameter = MakeVoxelShared<FVoxelSculptStorageQueryParameter>();
		Parameter->SurfaceToWorld = FVoxelTransformRef::Make(*TargetActor);
		Parameter->Data = TargetActor->SculptStorageComponent->GetData();
		Parameter->VoxelSize = VoxelSize;
		Parameter->Compute = Compute;
		QueryParameters->Add(Parameter);
	}

	const TOptional<FVoxelFloatBuffer> SurfaceDistances = FVoxelTaskGroup::TryRunSynchronously(NodeRuntime->GetContext(), [&]
	{
		const FVoxelQuery Query = FVoxelQuery::Make(
			FVoxelQueryContext::Make(TargetActor->GetRuntime()->GetRuntimeInfoRef().AsShared()),
			QueryParameters,
			FVoxelDependencyTracker::Create("VoxelSculpt"))
			.MakeNewQuery(NodeRuntime->GetContext());

		const TVoxelFutureValue<FVoxelSurface> Surface = NodeRuntime->GetNodeRuntime().Get(NodeRuntime->Node.NewSurfacePin, Query);
		return
			MakeVoxelTask()
			.Dependency(Surface)
			.Execute<FVoxelFloatBuffer>([=]
			{
				return Surface.Get_CheckCompleted().GetDistance(Query);
			});
	});

	if (!ensure(SurfaceDistances))
	{
		return;
	}

	if (SurfaceDistances->Num() != 1 &&
		SurfaceDistances->Num() != IntBounds.Count_SmallBox())
	{
		VOXEL_MESSAGE(Error, "{0}: Surface has a different buffer size than Positions", NodeRuntime.Get());
		return;
	}

	TVoxelArray<float> Distances;
	FVoxelUtilities::SetNumFast(Distances, IntBounds.Count_SmallBox());

	SurfaceDistances->GetStorage().CopyTo(Distances);

	for (float& Distance : Distances)
	{
		Distance /= VoxelSize;
	}

	TargetActor->SculptStorageComponent->GetData()->SetDistances(IntBounds, Distances);
	TargetActor->MarkPackageDirty();
}