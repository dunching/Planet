// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "MarchingCube/VoxelMarchingCubeExecNode.h"
#include "MarchingCube/VoxelMarchingCubeNodes.h"
#include "MarchingCube/VoxelMarchingCubeMesh.h"
#include "VoxelRuntime.h"
#include "VoxelSettings.h"
#include "VoxelDebugNode.h"
#include "VoxelGradientNodes.h"
#include "VoxelDetailTextureNodes.h"
#include "VoxelScreenSizeChunkSpawner.h"
#include "Rendering/VoxelMeshComponent.h"

FVoxelNodeAliases::TValue<FVoxelMarchingCubeExecNodeMesh> FVoxelMarchingCubeExecNode::CreateMesh(
	const FVoxelQuery& InQuery,
	const float VoxelSize,
	const int32 ChunkSize,
	const FVoxelBox& Bounds) const
{
	checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(this));

	const TSharedRef<FVoxelDebugQueryParameter> DebugParameter = MakeVoxelShared<FVoxelDebugQueryParameter>();

	const FVoxelQuery Query = INLINE_LAMBDA
	{
		const TSharedRef<FVoxelQueryParameters> Parameters = InQuery.CloneParameters();
		Parameters->Add(DebugParameter);
		return InQuery.EnterScope(*this).MakeNewQuery(Parameters);
	};

	const TValue<FVoxelSurface> FutureSurface = INLINE_LAMBDA
	{
		const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
		Parameters->Add<FVoxelQueryChannelBoundsQueryParameter>().Bounds = Bounds;
		return GetNodeRuntime().Get(SurfacePin, Query.MakeNewQuery(Parameters));
	};

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
			return true;
		};
		VOXEL_CALL_NODE_BIND(PerfectTransitionsPin)
		{
			return GetNodeRuntime().Get(PerfectTransitionsPin, Query);
		};
		VOXEL_CALL_NODE_BIND(EnableDistanceChecksPin)
		{
			return true;
		};
		VOXEL_CALL_NODE_BIND(DistanceChecksTolerancePin)
		{
			return GetNodeRuntime().Get(DistanceChecksTolerancePin, Query);
		};
	};

	const TValue<FVoxelMesh> Mesh = VOXEL_CALL_NODE(FVoxelNode_CreateMarchingCubeMesh, MeshPin, Query)
	{
		VOXEL_CALL_NODE_BIND(SurfacePin, MarchingCubeSurface)
		{
			return MarchingCubeSurface;
		};

		VOXEL_CALL_NODE_BIND(MaterialPin, DebugParameter, FutureSurface)
		{
			FindVoxelQueryParameter(FVoxelLODQueryParameter, LODQueryParameter);

			const TValue<FVoxelMaterialParameter> FutureMaterialIdMaterialParameter = VOXEL_CALL_NODE(FVoxelNode_MakeMaterialIdDetailTextureParameter, ParameterPin, Query)
			{
				CalleeNode.bIsMain = true;

				VOXEL_CALL_NODE_BIND(SurfacePin)
				{
					return GetNodeRuntime().Get(SurfacePin, Query);
				};
				VOXEL_CALL_NODE_BIND(TexturePin)
				{
					return GetNodeRuntime().Get(MaterialIdDetailTexturePin, Query);
				};
			};

			const int32 LOD = LODQueryParameter->LOD;
			const TValue<FVoxelMaterial> MaterialTemplate = INLINE_LAMBDA
			{
				VOXEL_SCOPE_LOCK(DebugParameter->CriticalSection);

				if (DebugParameter->Entries_RequiresLock.Num() == 0)
				{
					return GetNodeRuntime().Get(MaterialPin, Query);
				}

				TVoxelArray<FVoxelGraphNodeRef> NodeRefs;
				TVoxelArray<TSharedPtr<const TVoxelComputeValue<FVoxelFloatBuffer>>> ChannelComputes;
				for (const auto& It : DebugParameter->Entries_RequiresLock)
				{
					const FVoxelDebugQueryParameter::FEntry& Entry = It.Value;
					if (Entry.Type.Is<FVoxelFloatBuffer>())
					{
						NodeRefs.Add(Entry.NodeRef);
						ChannelComputes.Add(ReinterpretCastSharedPtr<const TVoxelComputeValue<FVoxelFloatBuffer>>(Entry.Compute));
					}
					else if (Entry.Type.Is<FVoxelVector2DBuffer>())
					{
						NodeRefs.Add(Entry.NodeRef);
						NodeRefs.Add(Entry.NodeRef);

						ChannelComputes.Add(MakeVoxelShared<TVoxelComputeValue<FVoxelFloatBuffer>>([Compute = Entry.Compute](const FVoxelQuery& Query)
						{
							const FVoxelFutureValue Value = (*Compute)(Query);
							return
								MakeVoxelTask()
								.Dependency(Value)
								.Execute<FVoxelFloatBuffer>([=]
								{
									return Value.Get_CheckCompleted<FVoxelVector2DBuffer>().X;
								});
						}));
						ChannelComputes.Add(MakeVoxelShared<TVoxelComputeValue<FVoxelFloatBuffer>>([Compute = Entry.Compute](const FVoxelQuery& Query)
						{
							const FVoxelFutureValue Value = (*Compute)(Query);
							return
								MakeVoxelTask()
								.Dependency(Value)
								.Execute<FVoxelFloatBuffer>([=]
								{
									return Value.Get_CheckCompleted<FVoxelVector2DBuffer>().Y;
								});
						}));
					}
					else if (Entry.Type.Is<FVoxelVectorBuffer>())
					{
						NodeRefs.Add(Entry.NodeRef);
						NodeRefs.Add(Entry.NodeRef);
						NodeRefs.Add(Entry.NodeRef);

						ChannelComputes.Add(MakeVoxelShared<TVoxelComputeValue<FVoxelFloatBuffer>>([Compute = Entry.Compute](const FVoxelQuery& Query)
						{
							const FVoxelFutureValue Value = (*Compute)(Query);
							return
								MakeVoxelTask()
								.Dependency(Value)
								.Execute<FVoxelFloatBuffer>([=]
								{
									return Value.Get_CheckCompleted<FVoxelVectorBuffer>().X;
								});
						}));
						ChannelComputes.Add(MakeVoxelShared<TVoxelComputeValue<FVoxelFloatBuffer>>([Compute = Entry.Compute](const FVoxelQuery& Query)
						{
							const FVoxelFutureValue Value = (*Compute)(Query);
							return
								MakeVoxelTask()
								.Dependency(Value)
								.Execute<FVoxelFloatBuffer>([=]
								{
									return Value.Get_CheckCompleted<FVoxelVectorBuffer>().Y;
								});
						}));
						ChannelComputes.Add(MakeVoxelShared<TVoxelComputeValue<FVoxelFloatBuffer>>([Compute = Entry.Compute](const FVoxelQuery& Query)
						{
							const FVoxelFutureValue Value = (*Compute)(Query);
							return
								MakeVoxelTask()
								.Dependency(Value)
								.Execute<FVoxelFloatBuffer>([=]
								{
									return Value.Get_CheckCompleted<FVoxelVectorBuffer>().Z;
								});
						}));
					}
				}

				if (ChannelComputes.Num() == 0)
				{
					return GetNodeRuntime().Get(MaterialPin, Query);
				}

				if (ChannelComputes.Num() > 3)
				{
					VOXEL_MESSAGE(Error, "{0}: More than 3 channels being debugged at once: {1}", this, NodeRefs);
					ChannelComputes.SetNum(3);
					ChannelComputes.SetNum(3);
				}

				TVoxelArray<TValue<FVoxelMaterialParameter>> Parameters;
				for (int32 Index = 0; Index < ChannelComputes.Num(); Index++)
				{
					const TSharedPtr<const TVoxelComputeValue<FVoxelFloatBuffer>> Compute = ChannelComputes[Index];

					Parameters.Add(VOXEL_CALL_NODE(FVoxelNode_MakeFloatDetailTextureParameter, ParameterPin, Query)
					{
						VOXEL_CALL_NODE_BIND(FloatPin, Compute)
						{
							return (*Compute)(Query);
						};
						VOXEL_CALL_NODE_BIND(TexturePin, Index)
						{
							return VOXEL_ON_COMPLETE_GAME_THREAD(Index)
							{
								if (!GetDefault<UVoxelSettings>()->MarchingCubeDebugDetailTextures.IsValidIndex(Index))
								{
									return {};
								}

								UVoxelFloatDetailTexture* Texture = GetDefault<UVoxelSettings>()->MarchingCubeDebugDetailTextures[Index].LoadSynchronous();
								if (!Texture)
								{
									return {};
								}

								return FVoxelFloatDetailTextureRef{ GVoxelDetailTextureManager->FindOrAddPool_GameThread(*Texture) };
							};
						};
					});
				}

				return
					MakeVoxelTask()
					.Thread(EVoxelTaskThread::GameThread)
					.Dependencies(Parameters)
					.Execute<FVoxelMaterial>([=]
					{
						FVoxelMaterial Result;
						Result.ParentMaterial = FVoxelMaterialRef::Make(GetDefault<UVoxelSettings>()->MarchingCubeDebugMaterial.LoadSynchronous());
						for (const TValue<FVoxelMaterialParameter>& Parameter : Parameters)
						{
							Result.Parameters.Add(Parameter.GetShared_CheckCompleted());
						}
						return Result;
					});
			};

			const TValue<FVoxelMaterial> Material =
				MakeVoxelTask()
				.Dependencies(FutureSurface, MaterialTemplate, FutureMaterialIdMaterialParameter)
				.Execute<FVoxelMaterial>([=]
				{
					const FVoxelSurface& Surface = FutureSurface.Get_CheckCompleted();

					TVoxelArray<TValue<FVoxelMaterialParameter>> Parameters;
					for (const auto& It : Surface.NameToAttribute)
					{
						const FVoxelSurface::FAttribute& Attribute = It.Value;
						if (!Attribute.DetailTexturePool.IsValid() ||
							!ensure(Attribute.Compute))
						{
							continue;
						}

						if (Attribute.InnerType.Is<float>())
						{
							Parameters.Add(VOXEL_CALL_NODE(FVoxelNode_MakeFloatDetailTextureParameter, ParameterPin, Query)
							{
								VOXEL_CALL_NODE_BIND(FloatPin, Attribute)
								{
									return TValue<FVoxelFloatBuffer>((*Attribute.Compute)(Query));
								};
								VOXEL_CALL_NODE_BIND(TexturePin, Attribute)
								{
									return FVoxelFloatDetailTextureRef{ Attribute.DetailTexturePool };
								};
							});
						}
						else if (Attribute.InnerType.Is<FLinearColor>())
						{
							Parameters.Add(VOXEL_CALL_NODE(FVoxelNode_MakeColorDetailTextureParameter, ParameterPin, Query)
							{
								VOXEL_CALL_NODE_BIND(ColorPin, Attribute)
								{
									return TValue<FVoxelLinearColorBuffer>((*Attribute.Compute)(Query));
								};
								VOXEL_CALL_NODE_BIND(TexturePin, Attribute)
								{
									return FVoxelColorDetailTextureRef{ Attribute.DetailTexturePool };
								};
							});
						}
						else
						{
							ensure(false);
						}
					}

					return
						MakeVoxelTask()
						.Dependencies(Parameters)
						.Execute<FVoxelMaterial>([=]
						{
							if (!FutureMaterialIdMaterialParameter.Get_CheckCompleted().IsA<FVoxelDetailTextureParameter>() ||
								!FutureMaterialIdMaterialParameter.Get_CheckCompleted().AsChecked<FVoxelDetailTextureParameter>().Pool)
							{
								VOXEL_MESSAGE(Error, "{0}: MaterialId detail texture is null", this);
							}

							FVoxelMaterial Result = MaterialTemplate.Get_CheckCompleted();
							Result.Parameters.Add(FutureMaterialIdMaterialParameter.GetShared_CheckCompleted());
							for (const TValue<FVoxelMaterialParameter>& Parameter : Parameters)
							{
								Result.Parameters.Add(Parameter.GetShared_CheckCompleted());
							}
							return Result;
						});
				});

			if (LOD == 0)
			{
				return Material;
			}

			const TValue<FVoxelMaterialParameter> NormalMaterialParameter = VOXEL_CALL_NODE(FVoxelNode_MakeNormalDetailTextureParameter, ParameterPin, Query)
			{
				CalleeNode.bIsMain = true;

				VOXEL_CALL_NODE_BIND(NormalPin)
				{
					return VOXEL_CALL_NODE(FVoxelNode_GetGradient, GradientPin, Query)
					{
						VOXEL_CALL_NODE_BIND(ValuePin)
						{
							const TValue<FVoxelSurface> Surface = GetNodeRuntime().Get(SurfacePin, Query);
							return VOXEL_ON_COMPLETE(Surface)
							{
								return Surface->GetDistance(Query);
							};
						};
					};
				};
				VOXEL_CALL_NODE_BIND(TexturePin)
				{
					return GetNodeRuntime().Get(NormalDetailTexturePin, Query);
				};
			};

			return VOXEL_ON_COMPLETE(LOD, Material, NormalMaterialParameter)
			{
				if (!NormalMaterialParameter->IsA<FVoxelDetailTextureParameter>() ||
					!NormalMaterialParameter->AsChecked<FVoxelDetailTextureParameter>().Pool)
				{
					VOXEL_MESSAGE(Error, "{0}: Normal detail texture is null", this);
				}

				FVoxelMaterial Result = *Material;
				Result.Parameters.Add(NormalMaterialParameter);
				return Result;
			};
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
			return GetNodeRuntime().Get(GenerateDistanceFieldsPin, Query);
		};

		VOXEL_CALL_NODE_BIND(DistanceFieldBiasPin)
		{
			return GetNodeRuntime().Get(DistanceFieldBiasPin, Query);
		};
	};

	const TValue<FVoxelMeshSettings> MeshSettings = GetNodeRuntime().Get(MeshSettingsPin, Query);
	const TValue<FBodyInstance> BodyInstance =
		Query.GetInfo(EVoxelQueryInfo::Query).IsGameWorld()
		? GetNodeRuntime().Get(BodyInstancePin, Query)
		// Always collide in editor for sculpting & drag/drop
		: FBodyInstance();

	return
		MakeVoxelTask(STATIC_FNAME("MarchingCubeSceneNode - CreateCollider"))
		.Dependencies(Mesh, MeshSettings, BodyInstance)
		.Execute<FVoxelMarchingCubeExecNodeMesh>([=]() -> TValue<FVoxelMarchingCubeExecNodeMesh>
		{
			const TSharedRef<FVoxelMarchingCubeExecNodeMesh> Result = MakeVoxelShared<FVoxelMarchingCubeExecNodeMesh>();
			if (Mesh.Get_CheckCompleted().GetStruct() == FVoxelMesh::StaticStruct())
			{
				return Result;
			}
			Result->Mesh = Mesh.GetShared_CheckCompleted();
			Result->MeshSettings = MeshSettings.GetShared_CheckCompleted();

			if (Query.GetInfo(EVoxelQueryInfo::Query).FindParameter<FVoxelRuntimeParameter_DisableCollision>() ||
				BodyInstance.Get_CheckCompleted().GetCollisionEnabled(false) == ECollisionEnabled::NoCollision)
			{
				return Result;
			}

			const TValue<FVoxelCollider> Collider = VOXEL_CALL_NODE(FVoxelNode_CreateMarchingCubeCollider, ColliderPin, Query)
			{
				VOXEL_CALL_NODE_BIND(SurfacePin, MarchingCubeSurface)
				{
					return MarchingCubeSurface;
				};

				VOXEL_CALL_NODE_BIND(PhysicalMaterialPin)
				{
					return GetNodeRuntime().Get(PhysicalMaterialPin, Query);
				};
			};

			return
				MakeVoxelTask(STATIC_FNAME("MarchingCubeSceneNode - Create"))
				.Dependencies(Collider)
				.Execute<FVoxelMarchingCubeExecNodeMesh>([=]
				{
					if (Collider.Get_CheckCompleted().GetStruct() == FVoxelCollider::StaticStruct())
					{
						// If mesh isn't empty collider shouldn't be either
						ensure(false);
						return Result;
					}

					Result->Collider = Collider.GetShared_CheckCompleted();
					Result->BodyInstance = BodyInstance.GetShared_CheckCompleted();
					return Result;
				});
		});
}

TVoxelUniquePtr<FVoxelExecNodeRuntime> FVoxelMarchingCubeExecNode::CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const
{
	if (!FApp::CanEverRender())
	{
		// Never create on server
		return nullptr;
	}

	return MakeVoxelUnique<FVoxelMarchingCubeExecNodeRuntime>(SharedThis);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMarchingCubeExecNodeRuntime::Create()
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
	if (!ensure(Runtime))
	{
		return;
	}

	ChunkSpawner = GetConstantPin(Node.ChunkSpawnerPin)->MakeSharedCopy();
	VoxelSize = GetConstantPin(Node.VoxelSizePin);

	if (ChunkSpawner->GetStruct() == StaticStructFast<FVoxelChunkSpawner>())
	{
		const TSharedRef<FVoxelScreenSizeChunkSpawner> ScreenSizeChunkSpawner = MakeVoxelShared<FVoxelScreenSizeChunkSpawner>();
		ScreenSizeChunkSpawner->LastChunkScreenSize = 1.f;
		ChunkSpawner = ScreenSizeChunkSpawner;
	}

	ChunkSpawner->PrivateVoxelSize = VoxelSize;
	ChunkSpawner->PrivateCreateChunkLambda = MakeWeakPtrLambda(this, [this](
		const int32 LOD,
		const int32 ChunkSize,
		const FVoxelBox& Bounds) -> TSharedPtr<FVoxelChunkRef>
	{
		const TSharedRef<FChunkInfo> ChunkInfo = MakeVoxelShared<FChunkInfo>(LOD, ChunkSize, Bounds);

		VOXEL_SCOPE_LOCK(ChunkInfos_CriticalSection);
		ChunkInfos.Add(ChunkInfo->ChunkId, ChunkInfo);

		return MakeVoxelShared<FVoxelChunkRef>(ChunkInfo->ChunkId, ChunkActionQueue);
	});

	ChunkSpawner->Initialize(*Runtime);
}

void FVoxelMarchingCubeExecNodeRuntime::Destroy()
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(ChunkInfos_CriticalSection);

	const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
	if (!Runtime)
	{
		// Already destroyed
		for (const auto& It : ChunkInfos)
		{
			It.Value->Mesh = {};
			It.Value->MeshComponent = {};
			It.Value->CollisionComponent = {};
			It.Value->FlushOnComplete();
		}
		ChunkInfos.Empty();
		return;
	}

	TArray<FVoxelChunkId> ChunkIds;
	ChunkInfos.GenerateKeyArray(ChunkIds);

	for (const FVoxelChunkId ChunkId : ChunkIds)
	{
		ProcessAction(&*Runtime, FVoxelChunkAction(EVoxelChunkAction::Destroy, ChunkId));
	}

	ensure(ChunkInfos.Num() == 0);
}

void FVoxelMarchingCubeExecNodeRuntime::Tick(FVoxelRuntime& Runtime)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(ChunkSpawner))
	{
		return;
	}

	ChunkSpawner->Tick(Runtime);

	ProcessMeshes(Runtime);
	ProcessActions(&Runtime, true);

	if (ProcessActionsGraphEvent.IsValid())
	{
		VOXEL_SCOPE_COUNTER("Wait");
		ProcessActionsGraphEvent->Wait();
	}
	ProcessActionsGraphEvent = TGraphTask<TVoxelGraphTask<ENamedThreads::AnyBackgroundThreadNormalTask>>::CreateTask().ConstructAndDispatchWhenReady(MakeWeakPtrLambda(this, [this]
	{
		ProcessActions(nullptr, false);
	}));
}

void FVoxelMarchingCubeExecNodeRuntime::FChunkInfo::FlushOnComplete()
{
	if (OnCompleteArray.Num() == 0)
	{
		return;
	}

	AsyncVoxelTask([OnCompleteArray = MoveTemp(OnCompleteArray)]
	{
		for (const TSharedPtr<const TVoxelUniqueFunction<void()>>& OnComplete : OnCompleteArray)
		{
			(*OnComplete)();
		}
	});
}

void FVoxelMarchingCubeExecNodeRuntime::ProcessMeshes(FVoxelRuntime& Runtime)
{
	VOXEL_FUNCTION_COUNTER();

	TArray<TSharedPtr<const TVoxelUniqueFunction<void()>>> OnCompleteArray;
	ON_SCOPE_EXIT
	{
		if (OnCompleteArray.Num() > 0)
		{
			AsyncVoxelTask([OnCompleteArray = MoveTemp(OnCompleteArray)]
			{
				for (const TSharedPtr<const TVoxelUniqueFunction<void()>>& OnComplete : OnCompleteArray)
				{
					(*OnComplete)();
				}
			});
		}
	};

	VOXEL_SCOPE_LOCK(ChunkInfos_CriticalSection);

	FQueuedMesh QueuedMesh;
	while (QueuedMeshes->Dequeue(QueuedMesh))
	{
		const TSharedPtr<FChunkInfo> ChunkInfo = ChunkInfos.FindRef(QueuedMesh.ChunkId);
		if (!ChunkInfo)
		{
			continue;
		}

		const TSharedPtr<const FVoxelMesh> Mesh = QueuedMesh.Mesh->Mesh;
		const TSharedPtr<const FVoxelCollider> Collider = QueuedMesh.Mesh->Collider;

		if (!Mesh)
		{
			Runtime.DestroyComponent(ChunkInfo->MeshComponent);
		}
		else
		{
			VOXEL_ENQUEUE_RENDER_COMMAND(SetTransitionMask_RenderThread)([Mesh, TransitionMask = ChunkInfo->TransitionMask](FRHICommandList& RHICmdList)
			{
				ConstCast(CastChecked<FVoxelMarchingCubeMesh>(*Mesh)).SetTransitionMask_RenderThread(RHICmdList, TransitionMask);
			});

			if (!ChunkInfo->MeshComponent.IsValid())
			{
				ChunkInfo->MeshComponent = Runtime.CreateComponent<UVoxelMeshComponent>();
			}

			UVoxelMeshComponent* Component = ChunkInfo->MeshComponent.Get();
			if (ensure(Component))
			{
				Component->SetRelativeLocation(ChunkInfo->Bounds.Min);
				Component->SetMesh(Mesh);
				QueuedMesh.Mesh->MeshSettings->ApplyToComponent(*Component);
			}

			const auto Box = ChunkInfo->Bounds.ToFBox();
			for (const auto& Iter : Runtime.OnChunkChangedMap)
			{
				if (Iter.Value)
				{
					Iter.Value(
						Box,
						ChunkInfo->LOD,
						ChunkInfo->ChunkSize
					);
				}
			}
		}

		if (!Collider)
		{
			Runtime.DestroyComponent(ChunkInfo->CollisionComponent);
		}
		else
		{
			if (!ChunkInfo->CollisionComponent.IsValid())
			{
				ChunkInfo->CollisionComponent = Runtime.CreateComponent<UVoxelCollisionComponent>();
			}

			UVoxelCollisionComponent* Component = ChunkInfo->CollisionComponent.Get();
			if (ensure(Component))
			{
				Component->SetRelativeLocation(Collider->GetOffset());
				Component->SetBodyInstance(*QueuedMesh.Mesh->BodyInstance);
				if (!IsGameWorld())
				{
					Component->BodyInstance.SetResponseToChannel(ECC_EngineTraceChannel6, ECR_Block);
				}
				Component->SetCollider(Collider);
			}
		}

		OnCompleteArray.Append(ChunkInfo->OnCompleteArray);
		ChunkInfo->OnCompleteArray.Empty();
	}
}

void FVoxelMarchingCubeExecNodeRuntime::ProcessActions(FVoxelRuntime* Runtime, const bool bIsInGameThread)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(ChunkInfos_CriticalSection);
	ensure(bIsInGameThread == IsInGameThread());
	ensure(bIsInGameThread == (Runtime != nullptr));

	const double StartTime = FPlatformTime::Seconds();

	FVoxelChunkAction Action;
	while ((bIsInGameThread ? ChunkActionQueue->GameQueue : ChunkActionQueue->AsyncQueue).Dequeue(Action))
	{
		ProcessAction(Runtime, Action);

		if (FPlatformTime::Seconds() - StartTime > 0.1f)
		{
			break;
		}
	}
}

void FVoxelMarchingCubeExecNodeRuntime::ProcessAction(FVoxelRuntime* Runtime, const FVoxelChunkAction& Action)
{
	checkVoxelSlow(ChunkInfos_CriticalSection.IsLocked());

	const TSharedPtr<FChunkInfo> ChunkInfo = ChunkInfos.FindRef(Action.ChunkId);
	if (!ChunkInfo)
	{
		return;
	}

	switch (Action.Action)
	{
	default: ensure(false);
	case EVoxelChunkAction::Compute:
	{
		VOXEL_SCOPE_COUNTER("Compute");

		TVoxelDynamicValueFactory<FVoxelMarchingCubeExecNodeMesh> Factory(STATIC_FNAME("Marching Cube Mesh"), [
			&Node = Node,
			VoxelSize = VoxelSize,
			ChunkSize = ChunkInfo->ChunkSize,
			Bounds = ChunkInfo->Bounds](const FVoxelQuery& Query)
		{
			checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(&Node));
			return Node.CreateMesh(Query, VoxelSize, ChunkSize, Bounds);
		});

		const TSharedRef<FVoxelQueryParameters> Parameters = MakeVoxelShared<FVoxelQueryParameters>();
		Parameters->Add<FVoxelLODQueryParameter>().LOD = ChunkInfo->LOD;

		ensure(!ChunkInfo->Mesh.IsValid());
		ChunkInfo->Mesh = Factory
			.AddRef(NodeRef)
			.Priority(FVoxelTaskPriority::MakeBounds(
				ChunkInfo->Bounds,
				GetConstantPin(Node.PriorityOffsetPin),
				GetWorld(),
				GetLocalToWorld()))
			.Compute(GetContext(), Parameters);

		ChunkInfo->Mesh.OnChanged([QueuedMeshes = QueuedMeshes, ChunkId = ChunkInfo->ChunkId](const TSharedRef<const FVoxelMarchingCubeExecNodeMesh>& NewMesh)
		{
			QueuedMeshes->Enqueue(FQueuedMesh{ ChunkId, NewMesh });
		});

		if (Action.OnComputeComplete)
		{
			ChunkInfo->OnCompleteArray.Add(Action.OnComputeComplete);
		}
	}
	break;
	case EVoxelChunkAction::SetTransitionMask:
	{
		VOXEL_SCOPE_COUNTER("SetTransitionMask");
		check(IsInGameThread());

		ChunkInfo->TransitionMask = Action.TransitionMask;

		if (UVoxelMeshComponent* Component = ChunkInfo->MeshComponent.Get())
		{
			if (const TSharedPtr<const FVoxelMesh> Mesh = Component->GetMesh())
			{
				ConstCast(CastChecked<FVoxelMarchingCubeMesh>(*Mesh)).SetTransitionMask_GameThread(Action.TransitionMask);
			}
		}
	}
	break;
	case EVoxelChunkAction::BeginDestroy:
	{
		VOXEL_SCOPE_COUNTER("BeginDestroy");

		ChunkInfo->Mesh = {};
	}
	break;
	case EVoxelChunkAction::Destroy:
	{
		VOXEL_SCOPE_COUNTER("Destroy");
		check(IsInGameThread());
		check(Runtime);

		ChunkInfo->Mesh = {};

		Runtime->DestroyComponent(ChunkInfo->MeshComponent);
		Runtime->DestroyComponent(ChunkInfo->CollisionComponent);

		ChunkInfo->FlushOnComplete();

		ChunkInfos.Remove(Action.ChunkId);

		ensure(ChunkInfo.GetSharedReferenceCount() == 1);
	}
	break;
	}
}