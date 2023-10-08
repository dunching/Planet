// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelQuery.h"

class UVoxelGraphInterface;
class UVoxelParameterContainer;
class IVoxelExecNodeRuntimeInterface;
struct FVoxelSubsystem;

using FOnChunkChanged = std::function<void(const FBox&, int32, int32)>;

class VOXELGRAPHCORE_API FVoxelRuntime
	: public TSharedFromThis<FVoxelRuntime>
	, public TVoxelRuntimeInfo<FVoxelRuntime>
{
public:
	static TSharedRef<FVoxelRuntime> Create(
		UObject& Instance,
		USceneComponent& RootComponent,
		const FVoxelRuntimeParameters& RuntimeParameters,
		UVoxelParameterContainer& ParameterContainer);
	~FVoxelRuntime();

	VOXEL_COUNT_INSTANCES();

	static bool CanBeCreated(bool bLog);

	void Destroy();
	void Tick();
	void AddReferencedObjects(FReferenceCollector& Collector);
	FVoxelOptionalBox GetBounds() const;

	TMap<uint8, FOnChunkChanged>OnChunkChangedMap;

public:
	FORCEINLINE const FVoxelRuntimeInfo& GetRuntimeInfoRef() const
	{
		return *RuntimeInfo;
	}
	FORCEINLINE const TVoxelSet<TWeakObjectPtr<USceneComponent>>& GetComponents() const
	{
		return Components;
	}
	FORCEINLINE IVoxelExecNodeRuntimeInterface& GetNodeRuntime() const
	{
		return *NodeRuntime;
	}

public:
	USceneComponent* CreateComponent(UClass* Class);
	void DestroyComponent_ReturnToPoolCalled(USceneComponent& Component);

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, USceneComponent>::Value>::Type>
	T* CreateComponent()
	{
		return CastChecked<T>(this->CreateComponent(T::StaticClass()), ECastCheckedType::NullAllowed);
	}
	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, USceneComponent>::Value && !std::is_same_v<T, USceneComponent>>::Type>
	void DestroyComponent(TWeakObjectPtr<T>& WeakComponent)
	{
		if (T* Component = WeakComponent.Get())
		{
			Component->ReturnToPool();
			this->DestroyComponent_ReturnToPoolCalled(*Component);
		}
		WeakComponent = {};
	}

private:
	TSharedPtr<FVoxelRuntimeInfo> RuntimeInfo;
	TSharedPtr<IVoxelExecNodeRuntimeInterface> NodeRuntime;

	TVoxelSet<TWeakObjectPtr<USceneComponent>> Components;
	TVoxelMap<UClass*, TArray<TWeakObjectPtr<USceneComponent>>> ComponentPools;

#if WITH_EDITOR
	struct FEditorTicker : public FVoxelTicker
	{
		FVoxelRuntime& Runtime;

		explicit FEditorTicker(FVoxelRuntime& Runtime)
			: Runtime(Runtime)
		{
		}

		virtual void Tick() override;
	};
	const FEditorTicker EditorTicker = FEditorTicker(*this);
#endif
};