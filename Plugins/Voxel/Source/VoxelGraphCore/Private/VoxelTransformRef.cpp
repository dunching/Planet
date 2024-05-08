// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelTransformRef.h"
#include "VoxelQuery.h"
#include "VoxelDependency.h"

class VOXELGRAPHCORE_API FVoxelTransformRefImpl
{
public:
	const FName Name;
	const TSharedRef<FVoxelDependency> Dependency;

	struct FNode
	{
		TWeakObjectPtr<const USceneComponent> Component;
		TWeakPtr<const IVoxelTransformProvider> Provider;
		bool bIsInverted = false;
		bool bIsProvider = false;
		FName DebugName;

		explicit FNode(const USceneComponent& Component)
			: Component(&Component)
		{
			ensure(IsInGameThread());

			if (const AActor* Owner = Component.GetOwner())
			{
				DebugName = Owner->GetName() + "." + Component.GetFName();
			}
			else
			{
				ensureVoxelSlow(false);
				DebugName = "<null>." + Component.GetFName();
			}
		}
		explicit FNode(const TSharedRef<const IVoxelTransformProvider>& Provider)
			: Provider(Provider)
			, bIsProvider(true)
			, DebugName(Provider->GetName())
		{
		}

		FORCEINLINE uint64 GetComponentOrProvider() const
		{
			return bIsProvider
				? ReinterpretCastRef<uint64>(GetWeakPtrObject_Unsafe(Provider))
				: ReinterpretCastRef<uint64>(MakeObjectKey(Component));
		}
		FORCEINLINE bool IsInverseOf(const FNode& Other) const
		{
			if (bIsInverted == Other.bIsInverted ||
				bIsProvider != Other.bIsProvider)
			{
				return false;
			}

			if (bIsProvider)
			{
				return Provider == Other.Provider;
			}
			else
			{
				return MakeObjectKey(Component) == MakeObjectKey(Other.Component);
			}
		}
	};
	const TVoxelArray<FNode> Nodes;

	FMatrix Transform = FMatrix::Identity;

	mutable FVoxelFastCriticalSection InverseCriticalSection;
	mutable TWeakPtr<const FVoxelTransformRefImpl> Inverse_RequiresLock;

	mutable FVoxelFastCriticalSection OnChangedCriticalSection;
	mutable TMulticastDelegate<void(const FMatrix& NewTransform)> OnChanged_RequiresLock;

private:
	explicit FVoxelTransformRefImpl(const TVoxelArray<FNode>& Nodes);
	static FName MakeName(const TVoxelArray<FNode>& InNodes);

	friend class FVoxelTransformRefManager;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTransformRefImpl::FVoxelTransformRefImpl(const TVoxelArray<FNode>& Nodes)
	: Name(MakeName(Nodes))
	, Dependency(FVoxelDependency::Create(STATIC_FNAME("TransformRef"), Name))
	, Nodes(Nodes)
{
	ensure(Nodes.Num() > 0);
}

FName FVoxelTransformRefImpl::MakeName(const TVoxelArray<FNode>& InNodes)
{
	ensure(InNodes.Num() > 0);

	FString Name;
	for (const FNode& Node : InNodes)
	{
		if (!Name.IsEmpty())
		{
			Name += TEXT(" -> ");
		}
		Name += Node.DebugName.ToString();
	}
	return FName(Name);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelTransformRefManager : public FVoxelSingleton
{
public:
	TVoxelArray<TSharedPtr<FVoxelTransformRefImpl>> TransformRefs_GameThread;
	TQueue<TSharedPtr<FVoxelTransformRefImpl>, EQueueMode::Mpsc> TransformRefQueue;

	TSharedRef<FVoxelTransformRefImpl> Make(const TVoxelArray<FVoxelTransformRefImpl::FNode>& Nodes)
	{
		ensure(Nodes.Num() > 0);
		VOXEL_SCOPE_LOCK(NodesToTransformRefCriticalSection);

		FNodeArray NodeArray;
		for (const FVoxelTransformRefImpl::FNode& Node : Nodes)
		{
			NodeArray.ComponentOrProviders.Add(Node.GetComponentOrProvider());
			NodeArray.IsInverted.Add(Node.bIsInverted);
		}

		if (const TSharedPtr<FVoxelTransformRefImpl> TransformRef = NodesToTransformRef.FindRef(NodeArray).Pin())
		{
			return TransformRef.ToSharedRef();
		}

		VOXEL_FUNCTION_COUNTER();

		for (auto It = NodesToTransformRef.CreateIterator(); It; ++It)
		{
			if (!It.Value().IsValid())
			{
				It.RemoveCurrent();
			}
		}

		// Try to find an accurate transform if we can, to avoid trashing all the work we do until the next game tick
		// This is also critical when working with no-dependency workflows (eg, TryRunSynchronously)
		FMatrix Transform = FMatrix::Identity;
		for (const FVoxelTransformRefImpl::FNode& Node : Nodes)
		{
			FNodeArray LocalNodeArray;
			LocalNodeArray.ComponentOrProviders.Add(Node.GetComponentOrProvider());
			LocalNodeArray.IsInverted.Add(false);

			FMatrix LocalTransform = FMatrix::Identity;
			if (const TSharedPtr<FVoxelTransformRefImpl> LocalTransformRef = NodesToTransformRef.FindRef(LocalNodeArray).Pin())
			{
				LocalTransform = LocalTransformRef->Transform;
			}
			else
			{
				if (!IsInGameThread())
				{
					// Transform will be out of date
					ensureVoxelSlow(false);
					continue;
				}

				if (Node.bIsProvider)
				{
					const TSharedPtr<const IVoxelTransformProvider> Provider = Node.Provider.Pin();
					if (!ensureVoxelSlow(Provider))
					{
						continue;
					}

					LocalTransform = Provider->GetTransform();
				}
				else
				{
					const USceneComponent* Component = Node.Component.Get();
					if (!ensureVoxelSlow(Component))
					{
						continue;
					}

					LocalTransform = Component->GetComponentTransform().ToMatrixWithScale();
				}
			}

			if (Node.bIsInverted)
			{
				Transform *= LocalTransform.Inverse();
			}
			else
			{
				Transform *= LocalTransform;
			}
		}

		const TSharedRef<FVoxelTransformRefImpl> TransformRef = MakeVoxelShareable(new (GVoxelMemory) FVoxelTransformRefImpl(Nodes));
		TransformRef->Transform = Transform;
		NodesToTransformRef.Add(NodeArray, TransformRef);
		TransformRefQueue.Enqueue(TransformRef);
		return TransformRef;
	}

	//~ Begin FVoxelSingleton Interface
	virtual void Tick() override
	{
		VOXEL_FUNCTION_COUNTER();

		{
			TSharedPtr<FVoxelTransformRefImpl> TransformRef;
			while (TransformRefQueue.Dequeue(TransformRef))
			{
				TransformRefs_GameThread.Add(TransformRef);
			}
		}

		VOXEL_SCOPE_COUNTER_FORMAT("Num = %d", TransformRefs_GameThread.Num());

		FVoxelDependencyInvalidationScope InvalidationScope;

		for (int32 Index = 0; Index < TransformRefs_GameThread.Num(); Index++)
		{
			TSharedPtr<FVoxelTransformRefImpl>& TransformRef = TransformRefs_GameThread[Index];
			if (TransformRef.GetSharedReferenceCount() == 1)
			{
				// Thread safe removal, make sure it's actually invalid
				const TWeakPtr<FVoxelTransformRefImpl> WeakPtr = TransformRef;
				TransformRef.Reset();
				TransformRef = WeakPtr.Pin();

				if (!TransformRef)
				{
					TransformRefs_GameThread.RemoveAtSwap(Index, 1, false);
					Index--;
					continue;
				}
			}

			FMatrix Transform = FMatrix::Identity;
			for (const FVoxelTransformRefImpl::FNode& Node : TransformRef->Nodes)
			{
				FMatrix LocalTransform = FMatrix::Identity;
				if (Node.bIsProvider)
				{
					const TSharedPtr<const IVoxelTransformProvider> Provider = Node.Provider.Pin();
					if (!Provider)
					{
						continue;
					}

					LocalTransform = Provider->GetTransform();
				}
				else
				{
					const USceneComponent* Component = Node.Component.Get();
					if (!Component)
					{
						continue;
					}

					LocalTransform = Component->GetComponentTransform().ToMatrixWithScale();
				}

				if (Node.bIsInverted)
				{
					Transform *= LocalTransform.Inverse();
				}
				else
				{
					Transform *= LocalTransform;
				}
			}

			if (TransformRef->Transform.Equals(Transform))
			{
				continue;
			}

			TransformRef->Transform = Transform;
			TransformRef->Dependency->Invalidate();

			TMulticastDelegate<void(const FMatrix& NewTransform)> OnChanged;
			{
				VOXEL_SCOPE_LOCK(TransformRef->OnChangedCriticalSection);
				OnChanged = TransformRef->OnChanged_RequiresLock;
			}

			VOXEL_SCOPE_COUNTER("OnChanged");
			OnChanged.Broadcast(Transform);
		}
	}
	//~ End FVoxelSingleton Interface

private:
	FVoxelFastCriticalSection NodesToTransformRefCriticalSection;

	struct FNodeArray
	{
		TVoxelArray<uint64, TVoxelInlineAllocator<3>> ComponentOrProviders;
		TVoxelBitArray<TVoxelInlineAllocator<1>> IsInverted;

		friend uint32 GetTypeHash(const FNodeArray& NodeArray)
		{
			ensureVoxelSlow(NodeArray.ComponentOrProviders.Num() < 32);

			uint64 Hash = FVoxelUtilities::MurmurHash64(NodeArray.IsInverted.GetWord(0));
			for (const uint64 ComponentOrProvider : NodeArray.ComponentOrProviders)
			{
				Hash ^= FVoxelUtilities::MurmurHash64(ComponentOrProvider);
			}
			return Hash;
		}
		bool operator==(const FNodeArray& Other) const
		{
			return
				ComponentOrProviders == Other.ComponentOrProviders &&
				IsInverted == Other.IsInverted;
		}
	};
	TVoxelMap<FNodeArray, TWeakPtr<FVoxelTransformRefImpl>> NodesToTransformRef;
};

FVoxelTransformRefManager* GVoxelTransformRefManager = MakeVoxelSingleton(FVoxelTransformRefManager);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTransformRef FVoxelTransformRef::Make(const AActor& Actor)
{
	const USceneComponent* RootComponent = Actor.GetRootComponent();
	if (!ensure(RootComponent))
	{
		return {};
	}
	return Make(*RootComponent);
}

FVoxelTransformRef FVoxelTransformRef::Make(const USceneComponent& Component)
{
	return FVoxelTransformRef(GVoxelTransformRefManager->Make({ FVoxelTransformRefImpl::FNode(Component) }));
}

FVoxelTransformRef FVoxelTransformRef::Make(const TSharedRef<const IVoxelTransformProvider>& Provider)
{
	return FVoxelTransformRef(GVoxelTransformRefManager->Make({ FVoxelTransformRefImpl::FNode(Provider) }));
}

void FVoxelTransformRef::NotifyTransformChanged(const USceneComponent& Component)
{
	VOXEL_FUNCTION_COUNTER();

	// TODO Do something smarter
	GVoxelTransformRefManager->Tick();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelTransformRef::IsIdentity() const
{
	return !Impl;
}

FMatrix FVoxelTransformRef::Get(const FVoxelQuery& Query) const
{
	if (!Impl)
	{
		return FMatrix::Identity;
	}

	// Make sure to keep the Impl alive so the dependency can be invalided
	Query.GetDependencyTracker().AddDependency(Impl->Dependency);
	Query.GetDependencyTracker().AddObjectToKeepAlive(Impl);
	return Impl->Transform;
}

FMatrix FVoxelTransformRef::Get_NoDependency() const
{
	if (!Impl)
	{
		return FMatrix::Identity;
	}

	return Impl->Transform;
}

FVoxelTransformRef FVoxelTransformRef::Inverse() const
{
	if (!Impl)
	{
		return {};
	}

	VOXEL_SCOPE_LOCK(Impl->InverseCriticalSection);

	if (const TSharedPtr<const FVoxelTransformRefImpl> Inverse = Impl->Inverse_RequiresLock.Pin())
	{
		return FVoxelTransformRef(Inverse.ToSharedRef());
	}

	TVoxelArray<FVoxelTransformRefImpl::FNode> Nodes;
	for (int32 Index = Impl->Nodes.Num() - 1; Index >= 0; Index--)
	{
		FVoxelTransformRefImpl::FNode Node = Impl->Nodes[Index];
		Node.bIsInverted = !Node.bIsInverted;
		Nodes.Add(Node);
	}

	const TSharedRef<FVoxelTransformRefImpl> Inverse = GVoxelTransformRefManager->Make(Nodes);
	Inverse->Inverse_RequiresLock = Impl;
	Impl->Inverse_RequiresLock = Inverse;
	return FVoxelTransformRef(Inverse);
}

FVoxelTransformRef FVoxelTransformRef::operator*(const FVoxelTransformRef& Other) const
{
	if (!Impl)
	{
		return Other;
	}
	if (!Other.Impl)
	{
		return *this;
	}

	TVoxelArray<FVoxelTransformRefImpl::FNode> Nodes;
	Nodes.Append(Impl->Nodes);

	for (const FVoxelTransformRefImpl::FNode& Node : Other.Impl->Nodes)
	{
		if (Nodes.Num() > 0 &&
			Nodes.Last().IsInverseOf(Node))
		{
			Nodes.Pop(false);
			continue;
		}

		Nodes.Add(Node);
	}

	if (Nodes.Num() == 0)
	{
		return {};
	}

	return FVoxelTransformRef(GVoxelTransformRefManager->Make(Nodes));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelTransformRef::AddOnChanged(const FOnChanged& OnChanged, const bool bFireNow) const
{
	if (Impl)
	{
		VOXEL_SCOPE_LOCK(Impl->OnChangedCriticalSection);
		Impl->OnChanged_RequiresLock.Add(OnChanged);
	}

	if (bFireNow)
	{
		(void)OnChanged.ExecuteIfBound(Get_NoDependency());
	}
}