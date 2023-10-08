// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelQuery.h"
#include "VoxelChannel.generated.h"

struct FStreamableHandle;
class FVoxelWorldChannel;
class FVoxelChannelManager;

USTRUCT(BlueprintType, DisplayName = "Voxel Channel")
struct VOXELGRAPHCORE_API FVoxelChannelName
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Config")
	FName Name = "Project.Surface";
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelChannelExposedDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Config")
	FName Name;

	UPROPERTY(EditAnywhere, Category = "Config")
	FVoxelPinType Type;

	UPROPERTY(EditAnywhere, Category = "Config")
	FVoxelPinValue DefaultValue;

	void Fixup();
};

UCLASS(meta = (VoxelAssetType, AssetColor=Blue))
class VOXELGRAPHCORE_API UVoxelChannelRegistry : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	TArray<FVoxelChannelExposedDefinition> Channels;

	void UpdateChannels();

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct VOXELGRAPHCORE_API FVoxelChannelDefinition
{
	FName Name;
	FVoxelPinType Type;
	FVoxelRuntimePinValue DefaultValue;

	FString ToString() const;
	FVoxelPinValue GetExposedDefaultValue() const;

	bool operator==(const FVoxelChannelDefinition& Other) const
	{
		return
			Name == Other.Name &&
			Type == Other.Type &&
			GetExposedDefaultValue() == Other.GetExposedDefaultValue();
	}
};

struct VOXELGRAPHCORE_API FVoxelBrushPriority
{
	union
	{
		struct
		{
			uint64 NodeHash : 16;
			// Actor/component hash, might not be deterministic
			uint64 InstanceHash : 16;
			uint64 GraphHash : 16;
			uint64 BasePriority : 16;
		};
		uint64 Raw = 0;
	};

	explicit FVoxelBrushPriority() = default;

	static FVoxelBrushPriority Max()
	{
		FVoxelBrushPriority Result;
		Result.Raw = MAX_uint64;
		return Result;
	}

	FORCEINLINE bool operator<(const FVoxelBrushPriority& Other) const
	{
		return Raw < Other.Raw;
	}
	FORCEINLINE bool operator<=(const FVoxelBrushPriority& Other) const
	{
		return Raw <= Other.Raw;
	}

	FORCEINLINE bool operator>(const FVoxelBrushPriority& Other) const
	{
		return Raw > Other.Raw;
	}
	FORCEINLINE bool operator>=(const FVoxelBrushPriority& Other) const
	{
		return Raw >= Other.Raw;
	}

	FORCEINLINE bool operator==(const FVoxelBrushPriority& Other) const
	{
		return Raw == Other.Raw;
	}
	FORCEINLINE bool operator!=(const FVoxelBrushPriority& Other) const
	{
		return Raw != Other.Raw;
	}
};
checkStatic(sizeof(FVoxelBrushPriority) == sizeof(uint64));

class VOXELGRAPHCORE_API FVoxelBrush
{
public:
	const FName DebugName;
	const FVoxelBrushPriority Priority;
	const FVoxelBox LocalBounds;
	const FVoxelTransformRef LocalToWorld;
	const FVoxelComputeValue Compute;

	VOXEL_COUNT_INSTANCES();

	FVoxelBrush(
		const FName DebugName,
		const FVoxelBrushPriority Priority,
		const FVoxelBox& LocalBounds,
		const FVoxelTransformRef& LocalToWorld,
		FVoxelComputeValue&& Compute)
		: DebugName(DebugName)
		, Priority(Priority)
		, LocalBounds(LocalBounds)
		, LocalToWorld(LocalToWorld)
		, Compute(MoveTemp(Compute))
	{
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_SPARSE_INDEX(FVoxelBrushId);

class VOXELGRAPHCORE_API FVoxelBrushRef
{
public:
	~FVoxelBrushRef();

private:
	TWeakPtr<FVoxelWorldChannel> WeakChannel;
	FVoxelBrushId BrushId;

	FVoxelBrushRef(const TSharedRef<FVoxelWorldChannel>& Channel, const FVoxelBrushId BrushId)
		: WeakChannel(Channel)
		, BrushId(BrushId)
	{
	}

	friend class FVoxelWorldChannel;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelRuntimeChannel : public TSharedFromThis<FVoxelRuntimeChannel>
{
public:
	const TSharedRef<FVoxelWorldChannel> WorldChannel;
	const FVoxelChannelDefinition Definition;
	const FVoxelTransformRef RuntimeLocalToWorld;

	VOXEL_COUNT_INSTANCES();

	TSharedPtr<const FVoxelBrush> GetNextBrush(
		const FVoxelQuery& Query,
		const FVoxelBox& Bounds,
		FVoxelBrushPriority Priority) const;

	FVoxelFutureValue Get(const FVoxelQuery& Query) const;

	template<typename T>
	TVoxelFutureValue<T> Get(const FVoxelQuery& Query) const
	{
		const FVoxelFutureValue Value = Get(Query);
		if (!ensure(Value.IsValid()) ||
			!ensure(Value.GetParentType().Is<T>()))
		{
			return {};
		}
		return TVoxelFutureValue<T>(Value);
	}

public:
	static FVoxelBrushPriority GetFullPriority(
		int32 Priority,
		const FString& GraphPath,
		const FString* NodeId,
		const FString& InstanceName);

private:
	const TSharedRef<FVoxelDependency> Dependency;
	mutable FVoxelFastCriticalSection CriticalSection;

	struct FRuntimeBrush
	{
		const TSharedRef<const FVoxelBrush> Brush;
		const FVoxelTransformRef BrushToRuntime;
		const FVoxelBrushPriority Priority;
		TOptional<FVoxelBox> RuntimeBounds_RequiresLock;

		FRuntimeBrush(
			const TSharedRef<const FVoxelBrush>& Brush,
			const FVoxelTransformRef& BrushToRuntime)
			: Brush(Brush)
			, BrushToRuntime(BrushToRuntime)
			, Priority(Brush->Priority)
		{
		}
	};
	TVoxelMap<FVoxelBrushId, TSharedPtr<FRuntimeBrush>> RuntimeBrushes_RequiresLock;

	FVoxelRuntimeChannel(
		const TSharedRef<FVoxelWorldChannel>& WorldChannel,
		const FVoxelTransformRef& RuntimeLocalToWorld);

	void AddBrush(
		FVoxelBrushId BrushId,
		const TSharedRef<const FVoxelBrush>& Brush);

	void RemoveBrush(
		FVoxelBrushId BrushId,
		const TSharedRef<const FVoxelBrush>& Brush);

	friend class FVoxelWorldChannel;
};

class VOXELGRAPHCORE_API FVoxelRuntimeChannelCache : public TSharedFromThis<FVoxelRuntimeChannelCache>
{
public:
	static TSharedRef<FVoxelRuntimeChannelCache> Create();

private:
	FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FName, TSharedPtr<FVoxelRuntimeChannel>> Channels_RequiresLock;

	FVoxelRuntimeChannelCache() = default;

	friend class FVoxelWorldChannel;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelWorldChannel : public TSharedFromThis<FVoxelWorldChannel>
{
public:
	const FVoxelChannelDefinition Definition;

	void AddBrush(
		const TSharedRef<const FVoxelBrush>& Brush,
		TSharedPtr<FVoxelBrushRef>& OutBrushRef);

	TSharedRef<FVoxelRuntimeChannel> GetRuntimeChannel(
		const FVoxelTransformRef& RuntimeLocalToWorld,
		FVoxelRuntimeChannelCache& Cache);

	void DrawBrushBounds(FObjectKey World) const;

private:
	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelArray<TWeakPtr<FVoxelRuntimeChannel>> WeakRuntimeChannels_RequiresLock;
	TVoxelSparseArray<TSharedPtr<const FVoxelBrush>, FVoxelBrushId> Brushes_RequiresLock;

	explicit FVoxelWorldChannel(const FVoxelChannelDefinition& Definition)
		: Definition(Definition)
	{
	}

	void RemoveBrush_RequiresLock(FVoxelBrushId BrushId);

	friend class FVoxelBrushRef;
	friend class FVoxelChannelManager;
	friend class FVoxelWorldChannelManager;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelWorldChannelManager : public IVoxelWorldSubsystem
{
public:
	GENERATED_VOXEL_WORLD_SUBSYSTEM_BODY(FVoxelWorldChannelManager);

	bool RegisterChannel(const FVoxelChannelDefinition& ChannelDefinition);
	TSharedPtr<FVoxelWorldChannel> FindChannel(FName Name);
	TArray<FName> GetValidChannelNames() const;

	//~ Begin IVoxelWorldSubsystem Interface
	virtual void Tick() override;
	//~ End IVoxelWorldSubsystem Interface

private:
	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FName, TSharedPtr<FVoxelWorldChannel>> Channels_RequiresLock;

	friend class FVoxelChannelManager;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern VOXELGRAPHCORE_API FVoxelChannelManager* GVoxelChannelManager;

class VOXELGRAPHCORE_API FVoxelChannelManager : public FVoxelSingleton
{
public:
	FSimpleMulticastDelegate OnChannelDefinitionsChanged_GameThread;

	bool IsReady(bool bLog) const;

	TArray<FName> GetValidChannelNames() const;
	TArray<const UObject*> GetChannelAssets() const;
	TOptional<FVoxelChannelDefinition> FindChannelDefinition(FName Name) const;

	void LogAllBrushes_GameThread();
	void LogAllChannels_GameThread();

	bool RegisterChannel(const FVoxelChannelDefinition& ChannelDefinition);
	void UpdateChannelsFromAsset_GameThread(
		const UObject* Asset,
		const FString& Prefix,
		const TArray<FVoxelChannelExposedDefinition>& Channels);
	void RemoveChannelsFromAsset_GameThread(const UObject* Asset);

	void ClearQueuedRefresh()
	{
		bRefreshQueued = false;
	}

	//~ Begin FVoxelSingleton Interface
	virtual void Initialize() override;
	virtual void Tick() override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End FVoxelSingleton Interface

private:
	bool bRefreshQueued = false;
	bool bChannelRegistriesLoaded = false;
	TVoxelArray<TSharedPtr<FStreamableHandle>> PendingHandles;

	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<const UObject*, TVoxelMap<FName, FVoxelChannelDefinition>> AssetToChannelDefinitions_RequiresLock;
};