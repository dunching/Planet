// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelTask.h"
#include "VoxelTaskGroup.h"

class FVoxelTaskExecutor;

extern VOXELGRAPHCORE_API float GVoxelThreadingPriorityDuration;
extern VOXELGRAPHCORE_API FVoxelTaskExecutor* GVoxelTaskExecutor;

class VOXELGRAPHCORE_API FVoxelTaskExecutor : public FVoxelSingleton
{
public:
	FEvent& Event = *FPlatformProcess::GetSynchEventFromPool();

	// Called when AddGroup is called with Groups.Num() == 0
	FSimpleMulticastDelegate OnBeginProcessing;
	// Called when we're done executing all groups
	FSimpleMulticastDelegate OnEndProcessing;

	FORCEINLINE bool IsExiting() const
	{
		return bIsExiting.Load();
	}

	int32 NumTasks() const
	{
		return Groups.Num();
	}
	void LogAllTasks();
	void AddGroup(const TSharedRef<FVoxelTaskGroup>& Group);

public:
	//~ Begin FVoxelSingleton Interface
	virtual void Initialize() override;
	virtual void Tick() override;
	//~ End FVoxelSingleton Interface

	void Tick_RenderThread(FRDGBuilder& GraphBuilder);

private:
	struct FTaskGroup
	{
		double PriorityValue = 0;
		SharedPointerInternals::TReferenceControllerBase<ESPMode::ThreadSafe>* ReferenceController = nullptr;

		struct FData
		{
			TWeakPtr<FVoxelTaskGroup> WeakGroup;
			FVoxelTaskPriority Priority;
		};
		TVoxelUniquePtr<FData> Data;

		FTaskGroup() = default;

		explicit FTaskGroup(const TSharedRef<FVoxelTaskGroup>& Group)
			: PriorityValue(Group->Priority.GetPriority())
		{
			Data = MakeVoxelUnique<FData>();
			Data->WeakGroup = Group;
			Data->Priority = Group->Priority;

			ReferenceController = GetWeakPtrReferenceController(Data->WeakGroup);
		}

		FORCEINLINE bool operator<(const FTaskGroup& Other) const
		{
			return PriorityValue < Other.PriorityValue;
		}
	};
	checkStatic(sizeof(FTaskGroup) == 24);

	class FTaskGroupArray
	{
	public:
		FTaskGroupArray();
		UE_NONCOPYABLE(FTaskGroupArray);

		FORCEINLINE int32 Num() const
		{
			return NumGroups.GetValue();
		}

		template<typename LambdaType>
		FORCEINLINE void ForeachGroup(LambdaType&& Lambda)
		{
			UpdatePrioritiesIfNeeded();

			CriticalSection.ReadLock();
			{
				checkVoxelSlow(!bSorted || Algo::IsSorted(Groups));
				checkVoxelSlow(Groups.Num() == ValidGroups.Num());

				ValidGroups.ForAllSetBits([&](const int32 Index)
				{
					const FTaskGroup& TaskGroup = Groups[Index];
					checkVoxelSlow(TaskGroup.ReferenceController);

					if (ReinterpretCastRef<int32>(TaskGroup.ReferenceController->SharedReferenceCount) == 0)
					{
						ValidGroups[Index] = false;
						return true;
					}

					const TSharedPtr<FVoxelTaskGroup> Group = TaskGroup.Data->WeakGroup.Pin();
					if (!Group)
					{
						ValidGroups[Index] = false;
						return true;
					}

					return Lambda(*Group);
				});
			}
			CriticalSection.ReadUnlock();
		}

		void Add(const TSharedRef<FVoxelTaskGroup>& NewGroup);
		void Reset();
		void UpdatePriorities();

		FORCEINLINE void UpdatePrioritiesIfNeeded()
		{
			if (LastPriorityComputeTime + GVoxelThreadingPriorityDuration < FPlatformTime::Seconds())
			{
				UpdatePriorities();
			}
		}

	private:
		FVoxelSharedCriticalSection CriticalSection;
		bool bSorted = true;
		double LastPriorityComputeTime = 0;
		TVoxelArray<FTaskGroup> Groups;
		FVoxelBitArray32 ValidGroups;
		FThreadSafeCounter NumGroups;
	};

	class FThread : public FRunnable
	{
	public:
		FThread();
		virtual ~FThread() override;

		//~ Begin FRunnable Interface
		virtual uint32 Run() override;
		//~ End FRunnable Interface

	private:
		TVoxelAtomic<bool> bTimeToDie = false;
		FRunnableThread* Thread = nullptr;
	};

	TVoxelAtomic<bool> bIsExiting = false;
	double LastNoTasksTime = 0;
	bool bWasProcessingTaskLastFrame = false;

	FVoxelFastCriticalSection ThreadsCriticalSection;
	TVoxelArray<TUniquePtr<FThread>> Threads;

	FTaskGroupArray Groups;

	TQueue<TWeakPtr<FVoxelTaskGroup>, EQueueMode::Mpsc> GameGroupsQueue;

	TSharedPtr<FVoxelTaskGroup> GetGroupToProcess(const FThread* Thread);
};