// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelTransformRef.h"
#include "VoxelInvoker.generated.h"

class UVoxelInvokerComponent;

class VOXELGRAPHCORE_API FVoxelInvokerView : public TSharedFromThis<FVoxelInvokerView>
{
public:
	const FName Channel;
	const int32 ChunkSize;
	const int32 Offset;
	const FVoxelTransformRef LocalToWorld;

	FVoxelInvokerView(
		const FName Channel,
		const int32 ChunkSize,
		const int32 Offset,
		const FVoxelTransformRef& LocalToWorld)
		: Channel(Channel)
		, ChunkSize(ChunkSize)
		, Offset(Offset)
		, LocalToWorld(LocalToWorld)
	{
		ensure(ChunkSize > 0);
	}

	VOXEL_COUNT_INSTANCES();

	using FOnChanged = TDelegate<void(const TVoxelAddOnlySet<FIntVector>&)>;
	using FOnChangedMulticast = TMulticastDelegate<void(const TVoxelAddOnlySet<FIntVector>&)>;

	// OnAddChunk will be fired right away, might cause deadlock
	void Bind(
		const FOnChanged& OnAddChunk,
		const FOnChanged& OnRemoveChunk);

	// OnAddChunk will be fired async, cannot cause deadlock
	void Bind_Async(
		const FOnChanged& OnAddChunk,
		const FOnChanged& OnRemoveChunk);

	void Tick(
		const UWorld* World,
		const TVoxelSet<UVoxelInvokerComponent*>& InvokerComponents);

private:
	bool bTaskInProgress = false;
	FVoxelFastCriticalSection CriticalSection;
	TVoxelAddOnlySet<FIntVector> Chunks_RequiresLock;
	FOnChangedMulticast OnAddChunkMulticast_RequiresLock;
	FOnChangedMulticast OnRemoveChunkMulticast_RequiresLock;

	struct FInvoker
	{
		FVector Center = FVector(ForceInit);
		float Radius = 0.f;
	};
	void Tick_Async(TVoxelArray<FInvoker> Invokers);
};

class VOXELGRAPHCORE_API FVoxelInvokerManager : public IVoxelWorldSubsystem
{
public:
	GENERATED_VOXEL_WORLD_SUBSYSTEM_BODY(FVoxelInvokerManager);

	void LogInvokers();

	TSharedRef<FVoxelInvokerView> MakeView(
		FName Channel,
		int32 ChunkSize,
		int32 Offset,
		const FVoxelTransformRef& LocalToWorld);

	//~ Begin IVoxelWorldSubsystem Interface
	virtual void Tick() override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End IVoxelWorldSubsystem Interface

private:
	double LastTickTime = 0;
	TVoxelSet<UVoxelInvokerComponent*> InvokerComponents;

	struct FViewKey
	{
		FName Channel;
		int32 ChunkSize = 0;
		int32 Offset = 0;
		FVoxelTransformRef LocalToWorld;

		FORCEINLINE bool operator==(const FViewKey& Other) const
		{
			return
				Channel == Other.Channel &&
				ChunkSize == Other.ChunkSize &&
				Offset == Other.Offset &&
				LocalToWorld == Other.LocalToWorld;
		}
		FORCEINLINE friend uint32 GetTypeHash(const FViewKey & Key)
		{
			return
				GetTypeHash(Key.Channel) ^
				GetTypeHash(Key.ChunkSize) ^
				GetTypeHash(Key.Offset) ^
				GetTypeHash(Key.LocalToWorld);
		}
	};
	FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FViewKey, TSharedPtr<FVoxelInvokerView>> KeyToView_RequiresLock;

	friend UVoxelInvokerComponent;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UCLASS(HideCategories = ("Rendering", "Physics", "LOD", "Activation", "Collision", "Cooking", "AssetUserData"), meta = (BlueprintSpawnableComponent))
class VOXELGRAPHCORE_API UVoxelInvokerComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invoker")
	bool bEnabled = true;

	// In world space, not affected by scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invoker")
	float Radius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invoker", DisplayName = "Channels")
	TArray<FName> NewChannels = { "Default" };

	UPROPERTY()
	TSet<FName> Channels_DEPRECATED;

	//~ Begin UPrimitiveComponent Interface
	virtual void PostLoad() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End UPrimitiveComponent Interface
};