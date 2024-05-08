// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "MarchingCube/VoxelMarchingCubePreviewExecNode.h"
#include "MarchingCube/VoxelMarchingCubeNodes.h"
#include "VoxelRuntime.h"
#include "VoxelTaskGroup.h"
#include "VoxelDependency.h"
#include "VoxelGradientNodes.h"
#include "VoxelQueryChannelNode.h"
#include "Rendering/VoxelMeshComponent.h"
#include "Collision/VoxelCollisionComponent.h"

FVoxelNodeAliases::TValue<FVoxelMarchingCubeBrushPreviewMesh> FVoxelMarchingCubePreviewExecNode::CreateMesh(
	const FVoxelQuery& Query,
	const float VoxelSize,
	const int32 ChunkSize,
	const FVoxelBox& Bounds) const
{
	checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(this));

	const TSharedRef<FVoxelQueryParameters> SurfaceParameters = Query.CloneParameters();
	SurfaceParameters->Add<FVoxelQueryChannelBoundsQueryParameter>().Bounds = Bounds;
	const TValue<FVoxelSurface> FutureSurface = GetNodeRuntime().Get(SurfacePin, Query.MakeNewQuery(SurfaceParameters));

	const TValue<FVoxelMarchingCubeSurface> MarchingCubeSurface = VOXEL_CALL_NODE(FVoxelNode_GenerateMarchingCubeSurface, SurfacePin, Query)
	{
		VOXEL_CALL_NODE_BIND(DistancePin, FutureSurface)
		{
			return VOXEL_ON_COMPLETE(FutureSurface)
			{
				return FutureSurface->GetDistance(Query);
			};
		};
		VOXEL_CALL_NODE_BIND(VoxelSizePin, VoxelSize)
		{
			return VoxelSize;
		};
		VOXEL_CALL_NODE_BIND(ChunkSizePin, ChunkSize)
		{
			return ChunkSize;
		};
		VOXEL_CALL_NODE_BIND(BoundsPin, Bounds)
		{
			return Bounds;
		};
		VOXEL_CALL_NODE_BIND(EnableTransitionsPin)
		{
			return false;
		};
		VOXEL_CALL_NODE_BIND(PerfectTransitionsPin)
		{
			return false;
		};
		VOXEL_CALL_NODE_BIND(EnableDistanceChecksPin)
		{
			return false;
		};
		VOXEL_CALL_NODE_BIND(DistanceChecksTolerancePin)
		{
			return 0.f;
		};
	};

	const TValue<FVoxelMesh> Mesh = VOXEL_CALL_NODE(FVoxelNode_CreateMarchingCubeMesh, MeshPin, Query)
	{
		VOXEL_CALL_NODE_BIND(SurfacePin, MarchingCubeSurface)
		{
			return MarchingCubeSurface;
		};

		VOXEL_CALL_NODE_BIND(MaterialPin)
		{
			return GetNodeRuntime().Get(MaterialPin, Query);
		};

		VOXEL_CALL_NODE_BIND(DistancePin)
		{
			const TValue<FVoxelSurface> Surface = GetNodeRuntime().Get(SurfacePin, Query);
			return VOXEL_ON_COMPLETE(Surface)
			{
				return Surface->GetDistance(Query);
			};
		};

		VOXEL_CALL_NODE_BIND(GenerateDistanceFieldPin)
		{
			return false;
		};

		VOXEL_CALL_NODE_BIND(DistanceFieldBiasPin)
		{
			return 0.f;
		};
	};

	const TValue<FBodyInstance> BodyInstance = GetNodeRuntime().Get(BodyInstancePin, Query);

	const TValue<FVoxelCollider> Collider =
		Query.GetInfo(EVoxelQueryInfo::Query).FindParameter<FVoxelRuntimeParameter_DisableCollision>()
		? FVoxelCollider()
		: VOXEL_CALL_NODE(FVoxelNode_CreateMarchingCubeCollider, ColliderPin, Query)
		{
			VOXEL_CALL_NODE_BIND(SurfacePin, MarchingCubeSurface)
			{
				return MarchingCubeSurface;
			};

			VOXEL_CALL_NODE_BIND(PhysicalMaterialPin)
			{
				return {};
			};
		};

	const TValue<bool> OnlyDrawIfSelected = GetNodeRuntime().Get(OnlyDrawIfSelectedPin, Query);

	return
		MakeVoxelTask(STATIC_FNAME("FVoxelMarchingCubePreviewExecNode"))
		.Dependencies(Mesh, BodyInstance, Collider, OnlyDrawIfSelected)
		.Execute<FVoxelMarchingCubeBrushPreviewMesh>([=]
		{
			const TSharedRef<FVoxelMarchingCubeBrushPreviewMesh> Result = MakeVoxelShared<FVoxelMarchingCubeBrushPreviewMesh>();
			Result->Bounds = Bounds;
			Result->bOnlyDrawIfSelected = OnlyDrawIfSelected.Get_CheckCompleted();

			if (Mesh.Get_CheckCompleted().GetStruct() != FVoxelMesh::StaticStruct())
			{
				Result->Mesh = Mesh.GetShared_CheckCompleted();
			}

			Result->BodyInstance = BodyInstance.GetShared_CheckCompleted();

			if (Collider.Get_CheckCompleted().GetStruct() != FVoxelCollider::StaticStruct())
			{
				Result->Collider = Collider.GetShared_CheckCompleted();
			}

			return Result;
		});
}

TVoxelUniquePtr<FVoxelExecNodeRuntime> FVoxelMarchingCubePreviewExecNode::CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const
{
	return MakeVoxelUnique<FVoxelMarchingCubePreviewExecNodeRuntime>(SharedThis);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMarchingCubePreviewExecNodeRuntime::Create()
{
	VOXEL_FUNCTION_COUNTER();

	TVoxelDynamicValueFactory<FVoxelMarchingCubeBrushPreviewMesh> Factory(STATIC_FNAME("Marching Cube Preview Mesh"), [&Node = Node](const FVoxelQuery& InQuery)
	{
		checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(&Node));
		const FVoxelQuery Query = InQuery.EnterScope(Node);

		const TValue<int32> FutureSize = Node.GetNodeRuntime().Get(Node.SizePin, Query);
		const TValue<FVoxelBounds> FutureLocalBounds = Node.GetNodeRuntime().Get(Node.BoundsPin, Query);

		return
			MakeVoxelTask(STATIC_FNAME("FVoxelMarchingCubePreviewExecNodeRuntime"))
			.Dependencies(FutureSize, FutureLocalBounds)
			.Execute<FVoxelMarchingCubeBrushPreviewMesh>([=, &Node]
			{
				const TValue<FVoxelSurface> Surface = FutureLocalBounds.Get_CheckCompleted().IsValid()
					? FVoxelSurface()
					: Node.GetNodeRuntime().Get(Node.SurfacePin, Query);

				return
					MakeVoxelTask(STATIC_FNAME("FVoxelMarchingCubePreviewExecNodeRuntime"))
					.Dependency(Surface)
					.Execute<FVoxelMarchingCubeBrushPreviewMesh>([=, &Node]() -> TValue<FVoxelMarchingCubeBrushPreviewMesh>
					{
						const int32 Size = FMath::Clamp(2 * FMath::DivideAndRoundUp(FutureSize.Get_CheckCompleted(), 2), 4, 128);

						FVoxelBounds LocalBoundsRef = FutureLocalBounds.Get_CheckCompleted();
						if (!LocalBoundsRef.IsValid())
						{
							LocalBoundsRef = Surface.Get_CheckCompleted().Bounds;
						}

						if (!LocalBoundsRef.IsValid())
						{
							VOXEL_MESSAGE(Error, "{0}: Invalid bounds", Node);
							return {};
						}
						if (LocalBoundsRef.IsInfinite())
						{
							VOXEL_MESSAGE(Error, "{0}: Infinite bounds", Node);
							return {};
						}

						FVoxelBox LocalBounds = LocalBoundsRef.GetBox(Query, Query.GetQueryToWorld());

						const float VoxelSize = FMath::CeilToFloat(LocalBounds.Size().GetAbsMax() / Size);
						LocalBounds.Min = FVoxelUtilities::FloorToFloat(LocalBounds.Min);
						LocalBounds.Max = LocalBounds.Min + VoxelSize * Size;

						const TSharedRef<FVoxelQueryParameters> LocalParameters = Query.CloneParameters();
						LocalParameters->Add<FVoxelLODQueryParameter>().LOD = 0;

						return Node.CreateMesh(Query.MakeNewQuery(LocalParameters), VoxelSize, Size, LocalBounds);
					});
			});
	});

	MeshValue = Factory
		.AddRef(NodeRef)
		.Compute(GetContext());

	MeshValue.OnChanged_GameThread(MakeWeakPtrLambda(this, [this](const TSharedRef<const FVoxelMarchingCubeBrushPreviewMesh>& Mesh)
	{
		const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
		if (!ensureVoxelSlow(Runtime))
		{
			return;
		}

		if (!Mesh->Mesh)
		{
			Runtime->DestroyComponent(WeakMeshComponent);
		}
		else
		{
			UVoxelMeshComponent* Component = WeakMeshComponent.Get();
			if (!Component)
			{
				Component = Runtime->CreateComponent<UVoxelMeshComponent>();
				WeakMeshComponent = Component;
			}
			if (!ensure(Component))
			{
				return;
			}

			Component->bOnlyDrawIfSelected = Mesh->bOnlyDrawIfSelected;
			Component->SetRelativeLocation(Mesh->Bounds.Min);
			Component->SetMesh(Mesh->Mesh);
		}

		if (!Mesh->Collider)
		{
			Runtime->DestroyComponent(WeakCollisionComponent);
		}
		else
		{
			UVoxelCollisionComponent* Component = WeakCollisionComponent.Get();
			if (!Component)
			{
				Component = Runtime->CreateComponent<UVoxelCollisionComponent>();
				WeakCollisionComponent = Component;
			}
			if (!ensure(Component))
			{
				return;
			}

			Component->SetRelativeLocation(Mesh->Collider->GetOffset());
			Component->SetCollider(Mesh->Collider);

			if (IsGameWorld())
			{
				Component->SetBodyInstance(*Mesh->BodyInstance);
			}
			else
			{
				Component->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
				Component->BodyInstance.SetResponseToChannel(ECC_EngineTraceChannel6, ECR_Block);
			}
		}
	}));
}

void FVoxelMarchingCubePreviewExecNodeRuntime::Destroy()
{
	VOXEL_FUNCTION_COUNTER();

	MeshValue = {};

	if (const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime())
	{
		Runtime->DestroyComponent(WeakMeshComponent);
		Runtime->DestroyComponent(WeakCollisionComponent);
	}
	else
	{
		if (UVoxelMeshComponent* Component = WeakMeshComponent.Get())
		{
			Component->DestroyComponent();
		}
		if (UVoxelCollisionComponent* Component = WeakCollisionComponent.Get())
		{
			Component->DestroyComponent();
		}
	}
}

FVoxelOptionalBox FVoxelMarchingCubePreviewExecNodeRuntime::GetBounds() const
{
	VOXEL_FUNCTION_COUNTER();

	FString Error;
	const TOptional<FVoxelBox> Bounds = FVoxelTaskGroup::TryRunSynchronously(GetContext(), [&]
	{
		const FVoxelQuery Query = FVoxelQuery::Make(
			GetContext(),
			MakeVoxelShared<FVoxelQueryParameters>(),
			FVoxelDependencyTracker::Create("DependencyTracker"));

		const TValue<FVoxelBounds> FutureLocalBounds = Node.GetNodeRuntime().Get(Node.BoundsPin, Query);

		return MakeVoxelTask()
			.Dependency(FutureLocalBounds)
			.Execute<FVoxelBox>([=]
			{
				const TValue<FVoxelSurface> Surface = FutureLocalBounds.Get_CheckCompleted().IsValid()
					? FVoxelSurface()
					: Node.GetNodeRuntime().Get(Node.SurfacePin, Query);

				return MakeVoxelTask()
					.Dependency(Surface)
					.Execute<FVoxelBox>([=]
					{
						FVoxelBounds LocalBounds = FutureLocalBounds.Get_CheckCompleted();
						if (!LocalBounds.IsValid())
						{
							LocalBounds = Surface.Get_CheckCompleted().Bounds;
						}

						// In world space
						return LocalBounds.GetBox_NoDependency(FMatrix::Identity);
					});
			});
	}, &Error);

	if (!Bounds)
	{
		VOXEL_MESSAGE(Error, "{0}: Failed to query bounds: {1}", Node, Error);
		return {};
	}

	return Bounds.GetValue();
}