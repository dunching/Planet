// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelInvoker.h"
#include "SceneManagement.h"
#include "PrimitiveSceneProxy.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, bool, GVoxelShowInvokers, false,
	"voxel.ShowInvokers",
	"");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, float, GVoxelInvokerTickRate, 0.05f,
	"voxel.InvokerTickRate",
	"Time between invoker ticks");

VOXEL_CONSOLE_WORLD_COMMAND(
	LogInvokers,
	"voxel.LogInvokers",
	"")
{
	FVoxelInvokerManager::Get(World)->LogInvokers();
}

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelInvokerView);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelInvokerView::Bind(
	const FOnChanged& OnAddChunk,
	const FOnChanged& OnRemoveChunk)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	(void)OnAddChunk.ExecuteIfBound(Chunks_RequiresLock);

	OnAddChunkMulticast_RequiresLock.Add(OnAddChunk);
	OnRemoveChunkMulticast_RequiresLock.Add(OnRemoveChunk);
}

void FVoxelInvokerView::Bind_Async(
	const FOnChanged& OnAddChunk,
	const FOnChanged& OnRemoveChunk)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	AsyncVoxelTask([OnAddChunk, Chunks = Chunks_RequiresLock]
	{
		(void)OnAddChunk.ExecuteIfBound(Chunks);
	});

	OnAddChunkMulticast_RequiresLock.Add(OnAddChunk);
	OnRemoveChunkMulticast_RequiresLock.Add(OnRemoveChunk);
}

void FVoxelInvokerView::Tick(
	const UWorld* World,
	const TVoxelSet<UVoxelInvokerComponent*>& InvokerComponents)
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelInvokerView::Tick Channel=%s NumInvokers=%d", *Channel.ToString(), InvokerComponents.Num());

	TVoxelArray<FInvoker> Invokers;
	Invokers.Reserve(InvokerComponents.Num());

	for (UVoxelInvokerComponent* InvokerComponent : InvokerComponents)
	{
		if (!ensure(InvokerComponent) ||
			!InvokerComponent->bEnabled ||
			!ToCompatibleVoxelArray(InvokerComponent->NewChannels).Contains(Channel))
		{
			continue;
		}

		Invokers.Add(FInvoker
		{
			InvokerComponent->GetComponentLocation(),
			InvokerComponent->Radius
		});
	}

	if (Channel == STATIC_FNAME("Camera"))
	{
		FVector Position = FVector::ZeroVector;
		if (FVoxelGameUtilities::GetCameraView(World, Position))
		{
			Invokers.Add(FInvoker
			{
				Position,
				0.f
			});
		}
	}

	if (!ensureVoxelSlow(!bTaskInProgress))
	{
		LOG_VOXEL(Warning,
			"Skipping invoker tick for channel %s (%d invokers): last tick still running. "
			"Try increasing the chunk size of nodes that are using this channel.",
			*Channel.ToString(),
			Invokers.Num());
		return;
	}

	ensure(!bTaskInProgress);
	bTaskInProgress = true;

	AsyncVoxelTask(MakeWeakPtrLambda(this, [this, Invokers = MoveTemp(Invokers)]
	{
		Tick_Async(Invokers);
	}));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelInvokerView::Tick_Async(TVoxelArray<FInvoker> Invokers)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_COUNTER_FORMAT("%s Invokers.Num = %d", *Channel.ToString(), Invokers.Num());

	Invokers.Sort([](const FInvoker& A, const FInvoker& B)
	{
		return A.Radius > B.Radius;
	});

	{
		VOXEL_SCOPE_COUNTER("Remove duplicates");

		for (int32 IndexA = 0; IndexA < Invokers.Num(); IndexA++)
		{
			const FInvoker InvokerA = Invokers[IndexA];
			for (int32 IndexB = 0; IndexB < IndexA; IndexB++)
			{
				const FInvoker InvokerB = Invokers[IndexB];

				const float RadiusDelta = InvokerB.Radius - InvokerA.Radius;
				ensureVoxelSlow(RadiusDelta >= 0);

				if (FVector::DistSquared(InvokerA.Center, InvokerB.Center) > FMath::Square(RadiusDelta))
				{
					continue;
				}

				// If distance between two centers is less than the different of radius, then A is fully contained in B
				Invokers.RemoveAtSwap(IndexA);
				IndexA--;
				break;
			}
		}
	}

	struct FChunkedInvoker
	{
		FVector Center;
		double RadiusInChunks;
	};
	TVoxelArray<FChunkedInvoker> ChunkedInvokers;
	ChunkedInvokers.Reserve(Invokers.Num());
	{
		VOXEL_SCOPE_COUNTER("Make ChunkedInvokers");

		const FMatrix WorldToLocal = LocalToWorld.Get_NoDependency().Inverse();
		const float WorldToLocalScale = WorldToLocal.GetMaximumAxisScale();

		for (const FInvoker& Invoker : Invokers)
		{
			const FVector LocalPosition = WorldToLocal.TransformPosition(Invoker.Center);
			const float LocalRadius = (Invoker.Radius + Offset) * WorldToLocalScale;

			FChunkedInvoker ChunkedInvoker;
			ChunkedInvoker.Center = LocalPosition / ChunkSize;
			ChunkedInvoker.RadiusInChunks = LocalRadius / ChunkSize;
			ChunkedInvokers.Add(ChunkedInvoker);
		}
	}

	int64 MaxNumChunks = 0;
	for (const FChunkedInvoker& Invoker : ChunkedInvokers)
	{
		MaxNumChunks += FMath::Cube(2 * Invoker.RadiusInChunks + 1);
	}

	TVoxelAddOnlySet<FIntVector> Chunks;
	Chunks.Reserve(FMath::Min(MaxNumChunks, 32768));

	for (const FChunkedInvoker& Invoker : ChunkedInvokers)
	{
		VOXEL_SCOPE_COUNTER_FORMAT("Add invoker Radius=%f chunks", Invoker.RadiusInChunks);

		// Offset due to chunk position being the chunk lower corner
		constexpr double ChunkOffset = 0.5;
		// We want to check the chunk against invoker, not the chunk center
		// To avoid a somewhat expensive box-to-point distance, we offset the invoker radius by the chunk diagonal
		// (from chunk center to any chunk corner)
		constexpr double ChunkHalfDiagonal = UE_SQRT_3 / 2.;

		const FIntVector Min = FVoxelUtilities::FloorToInt(Invoker.Center - Invoker.RadiusInChunks - ChunkOffset);
		const FIntVector Max = FVoxelUtilities::CeilToInt(Invoker.Center + Invoker.RadiusInChunks - ChunkOffset);
		const double RadiusSquared = FMath::Square(Invoker.RadiusInChunks + ChunkHalfDiagonal);

		for (int32 X = Min.X; X <= Max.X; X++)
		{
			for (int32 Y = Min.Y; Y <= Max.Y; Y++)
			{
				for (int32 Z = Min.Z; Z <= Max.Z; Z++)
				{
					const double DistanceSquared = (FVector(X, Y, Z) + ChunkOffset - Invoker.Center).SizeSquared();
					if (DistanceSquared > RadiusSquared)
					{
						continue;
					}

					Chunks.Add(FIntVector(X, Y, Z));
				}

				if (Chunks.Num() > 1024 * 1024)
				{
					VOXEL_MESSAGE(Error, "More than 1M chunks generated by invoker channel {0} for chunk size {1}, abording",
						Channel,
						ChunkSize);

					ensure(bTaskInProgress);
					bTaskInProgress = false;
					return;
				}
			}
		}
	}

	TVoxelAddOnlySet<FIntVector> ChunksToAdd;
	TVoxelAddOnlySet<FIntVector> ChunksToRemove;
	ChunksToAdd.Reserve(Chunks.Num());
	ChunksToRemove.Reserve(Chunks_RequiresLock.Num()); // Not thread safe but fine for just Num

	FOnChangedMulticast OnAddChunkMulticast;
	FOnChangedMulticast OnRemoveChunkMulticast;
	{
		VOXEL_SCOPE_COUNTER("Diff");
		VOXEL_SCOPE_LOCK(CriticalSection);

		for (const FIntVector& Chunk : Chunks)
		{
			if (!Chunks_RequiresLock.Contains(Chunk))
			{
				ChunksToAdd.Add_NoRehash(Chunk);
			}
		}

		for (const FIntVector& Chunk : Chunks_RequiresLock)
		{
			if (!Chunks.Contains(Chunk))
			{
				ChunksToRemove.Add_NoRehash(Chunk);
			}
		}

		Chunks_RequiresLock = MoveTemp(Chunks);
		OnAddChunkMulticast = OnAddChunkMulticast_RequiresLock;
		OnRemoveChunkMulticast = OnRemoveChunkMulticast_RequiresLock;
	}

	if (ChunksToAdd.Num() > 0)
	{
		VOXEL_SCOPE_COUNTER("OnAddChunk");
		OnAddChunkMulticast.Broadcast(ChunksToAdd);
	}

	if (ChunksToRemove.Num() > 0)
	{
		VOXEL_SCOPE_COUNTER("OnRemoveChunk");
		OnRemoveChunkMulticast.Broadcast(ChunksToRemove);
	}

	ensure(bTaskInProgress);
	bTaskInProgress = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelInvokerManager::LogInvokers()
{
	VOXEL_FUNCTION_COUNTER();
	checkVoxelSlow(IsInGameThread());

	TVoxelAddOnlyMap<FName, TVoxelArray<const UVoxelInvokerComponent*>> ChannelToInvokers;
	for (const UVoxelInvokerComponent* InvokerComponent : InvokerComponents)
	{
		for (const FName Channel : InvokerComponent->NewChannels)
		{
			ChannelToInvokers.FindOrAdd(Channel).Add(InvokerComponent);
		}
	}

	for (const auto& It : ChannelToInvokers)
	{
		LOG_VOXEL(Log, "Channel '%s':", *It.Key.ToString());

		for (const UVoxelInvokerComponent* Invoker : It.Value)
		{
			LOG_VOXEL(Log, "\tEnabled: %s Radius: %f %s", *LexToString(Invoker->bEnabled), Invoker->Radius, *Invoker->GetPathName());
		}
	}
}

TSharedRef<FVoxelInvokerView> FVoxelInvokerManager::MakeView(
	const FName Channel,
	const int32 ChunkSize,
	const int32 Offset,
	const FVoxelTransformRef& LocalToWorld)
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	const FViewKey Key
	{
		Channel,
		ChunkSize,
		Offset,
		LocalToWorld
	};

	TSharedPtr<FVoxelInvokerView>& View = KeyToView_RequiresLock.FindOrAdd(Key);
	if (!View)
	{
		View = MakeVoxelShared<FVoxelInvokerView>(
			Channel,
			ChunkSize,
			Offset,
			LocalToWorld);
	}
	return View.ToSharedRef();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelInvokerManager::Tick()
{
	VOXEL_FUNCTION_COUNTER();

	const double Time = FPlatformTime::Seconds();
	if (Time < LastTickTime + GVoxelInvokerTickRate)
	{
		return;
	}
	LastTickTime = Time;

	VOXEL_SCOPE_LOCK(CriticalSection);

	for (auto It = KeyToView_RequiresLock.CreateIterator(); It; ++It)
	{
		if (It.Value().IsUnique())
		{
			It.RemoveCurrent();
			continue;
		}

		It.Value()->Tick(GetWorld(), InvokerComponents);
	}
}

void FVoxelInvokerManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();

	for (auto It = InvokerComponents.CreateIterator(); It; ++It)
	{
		UVoxelInvokerComponent* InvokerComponent = *It;
		Collector.AddReferencedObject(InvokerComponent);

		if (!ensure(InvokerComponent))
		{
			It.RemoveCurrent();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelInvokerComponent::PostLoad()
{
	Super::PostLoad();

	NewChannels.Append(Channels_DEPRECATED.Array());
}

void UVoxelInvokerComponent::OnRegister()
{
	Super::OnRegister();

	const UWorld* World = GetWorld();
	if (ensure(World))
	{
		ensure(!FVoxelInvokerManager::Get(World)->InvokerComponents.Contains(this));
		FVoxelInvokerManager::Get(World)->InvokerComponents.Add(this);
	}
}

void UVoxelInvokerComponent::OnUnregister()
{
	const UWorld* World = GetWorld();
	if (ensure(World))
	{
		ensure(FVoxelInvokerManager::Get(World)->InvokerComponents.Remove(this));
	}

	Super::OnUnregister();
}

FPrimitiveSceneProxy* UVoxelInvokerComponent::CreateSceneProxy()
{
	if (UE_BUILD_SHIPPING)
	{
		return nullptr;
	}

	class FVoxelInvokerSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		const float Radius;

		explicit FVoxelInvokerSceneProxy(const UVoxelInvokerComponent* Component)
			: FPrimitiveSceneProxy(Component)
			, Radius(Component->Radius)
		{
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			VOXEL_FUNCTION_COUNTER();

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (!(VisibilityMap & (1 << ViewIndex)))
				{
					continue;
				}

				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
				const FSceneView* View = Views[ViewIndex];
				const FMatrix LocalToWorld = GetLocalToWorld();

				const FLinearColor Color = GetViewSelectionColor(
					FColor::White,
					*View,
					IsSelected(),
					IsHovered(),
					false,
					IsIndividuallySelected());

				DrawWireSphere(
					PDI,
					LocalToWorld.GetOrigin(),
					Color,
					Radius,
					64,
					SDPG_World);
			}
		}
		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && IsSelected()) || GVoxelShowInvokers;
			Result.bDynamicRelevance = true;
			return Result;
		}
		virtual SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}
		virtual uint32 GetMemoryFootprint() const override
		{
			return sizeof(*this) + GetAllocatedSize();
		}
	};
	return new FVoxelInvokerSceneProxy(this);
}

FBoxSphereBounds UVoxelInvokerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FVector::ZeroVector, FVector(Radius), Radius).TransformBy(LocalToWorld);
}