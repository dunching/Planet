// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelTask.h"
#include "VoxelQuery.h"

extern VOXELGRAPHCORE_API const uint32 GVoxelTaskGroupTLS;

class VOXELGRAPHCORE_API FVoxelTaskGroup : public TSharedFromThis<FVoxelTaskGroup>
{
public:
	const FName Name;
	const FName InstanceStatName;
	const FName GraphStatName;
	const FName CallstackStatName;
	const bool bIsSynchronous;
	const FVoxelTaskPriority Priority;
	const TSharedRef<FVoxelTaskReferencer> Referencer;
	const TSharedRef<const FVoxelRuntimeInfo> RuntimeInfo;
	const TSharedRef<const FVoxelQueryContext> Context;

	VOXEL_COUNT_INSTANCES();

	static TSharedRef<FVoxelTaskGroup> Create(
		FName Name,
		const FVoxelTaskPriority& Priority,
		const TSharedRef<FVoxelTaskReferencer>& Referencer,
		const TSharedRef<const FVoxelQueryContext>& Context);

	static TSharedRef<FVoxelTaskGroup> CreateSynchronous(
		FName Name,
		const TSharedRef<FVoxelTaskReferencer>& Referencer,
		const TSharedRef<const FVoxelQueryContext>& Context);

	static TSharedRef<FVoxelTaskGroup> CreateSynchronous(
		const TSharedRef<FVoxelQueryContext>& Context);

public:
	static bool TryRunSynchronouslyGeneric(
		const TSharedRef<FVoxelQueryContext>& Context,
		TFunctionRef<void()> Lambda,
		FString* OutError = nullptr);

	template<typename LambdaType, typename T = typename decltype(DeclVal<LambdaType>()())::Type>
	static TOptional<T> TryRunSynchronously(
		const TSharedRef<FVoxelQueryContext>& Context,
		LambdaType&& Lambda,
		FString* OutError = nullptr)
	{
		TVoxelFutureValue<T> Future;

		if (!FVoxelTaskGroup::TryRunSynchronouslyGeneric(Context, [&]
			{
				Future = Lambda();
			}, OutError))
		{
			return {};
		}

		if (!ensure(Future.IsValid()) ||
			!ensure(Future.IsComplete()))
		{
			return {};
		}

		return Future.Get_CheckCompleted();
	}

	bool TryRunSynchronously(FString* OutError);
	bool TryRunSynchronously_Ensure();

public:
	static void StartAsyncTaskGeneric(
		FName Name,
		const TSharedRef<FVoxelQueryContext>& Context,
		const FVoxelPinType& Type,
		TVoxelUniqueFunction<FVoxelFutureValue()> MakeFuture,
		TFunction<void(const FVoxelRuntimePinValue&)> Callback);

	template<typename T>
	static void StartAsyncTask(
		const FName Name,
		const TSharedRef<FVoxelQueryContext>& Context,
		TVoxelUniqueFunction<TVoxelFutureValue<T>()> MakeFuture,
		TFunction<void(const typename TChooseClass<VoxelPassByValue<T>, T, TSharedRef<const T>>::Result&)> Callback)
	{
		FVoxelTaskGroup::StartAsyncTaskGeneric(
			Name,
			Context,
			FVoxelPinType::Make<T>(),
			MoveTemp(ReinterpretCastRef<TVoxelUniqueFunction<FVoxelFutureValue()>>(MakeFuture)),
			[Callback = MoveTemp(Callback)](const FVoxelRuntimePinValue& Value)
			{
				if constexpr (VoxelPassByValue<T>)
				{
					Callback(Value.Get<T>());
				}
				else
				{
					Callback(Value.GetSharedStruct<T>());
				}
			});
	}

public:
	FORCEINLINE FVoxelTaskReferencer& GetReferencer() const
	{
		return *Referencer;
	}

	void ProcessTask(TVoxelUniquePtr<FVoxelTask> TaskPtr);
	void AddPendingTask(TVoxelUniquePtr<FVoxelTask> TaskPtr);
	void OnDependencyComplete(FVoxelTask& Task);

	void LogTasks() const;

public:
	TVoxelAtomic<const void*> AsyncProcessor = nullptr;

	FORCEINLINE bool HasGameTasks() const { return !GameTasks.IsEmpty(); }
	FORCEINLINE bool HasRenderTasks() const { return !RenderTasks.IsEmpty(); }
	FORCEINLINE bool HasAsyncTasks() const { return !AsyncTasks.IsEmpty(); }

	void ProcessGameTasks();
	void ProcessRenderTasks(FRDGBuilder& GraphBuilder);
	void ProcessAsyncTasks();

public:
	FORCEINLINE bool ShouldExit() const
	{
		if (RuntimeInfo->ShouldExitTask())
		{
			return true;
		}
		if (bIsSynchronous)
		{
			return false;
		}
		return IsSharedFromThisUnique(this);
	}
	FORCEINLINE static FVoxelTaskGroup& Get()
	{
		void* TLSValue = FPlatformTLS::GetTlsValue(GVoxelTaskGroupTLS);
		checkVoxelSlow(TLSValue);
		return *static_cast<FVoxelTaskGroup*>(TLSValue);
	}
	FORCEINLINE static FVoxelTaskGroup* TryGet()
	{
		void* TLSValue = FPlatformTLS::GetTlsValue(GVoxelTaskGroupTLS);
		return static_cast<FVoxelTaskGroup*>(TLSValue);
	}

private:
	TQueue<TVoxelUniquePtr<FVoxelTask>, EQueueMode::Mpsc> GameTasks;
	TQueue<TVoxelUniquePtr<FVoxelTask>, EQueueMode::Mpsc> RenderTasks;
	TQueue<TVoxelUniquePtr<FVoxelTask>, EQueueMode::Mpsc> AsyncTasks;

	mutable FVoxelFastCriticalSection PendingTasksCriticalSection;
	TVoxelSparseArray<TVoxelUniquePtr<FVoxelTask>, FVoxelPendingTaskId> PendingTasks_RequiresLock;

	FVoxelTaskGroup(
		FName Name,
		bool bIsSynchronous,
		const FVoxelTaskPriority& Priority,
		const TSharedRef<FVoxelTaskReferencer>& Referencer,
		const TSharedRef<const FVoxelQueryContext>& Context);
};

struct VOXELGRAPHCORE_API FVoxelTaskGroupScope
{
public:
	FVoxelTaskGroupScope() = default;
	~FVoxelTaskGroupScope();

	UE_NODISCARD bool Initialize(FVoxelTaskGroup& NewGroup);

	FORCEINLINE FVoxelTaskGroup& GetGroup() const
	{
		return *Group;
	}

private:
	TSharedPtr<FVoxelTaskGroup> Group;
	void* PreviousTLS = nullptr;
	double StartTime = FPlatformTime::Seconds();
};