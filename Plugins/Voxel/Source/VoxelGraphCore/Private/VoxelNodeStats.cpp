// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelNodeStats.h"
#include "VoxelNode.h"
#include "VoxelCompiledGraph.h"
#include "EdGraph/EdGraphNode.h"

#if WITH_EDITOR
bool GVoxelEnableNodeStats = false;
TArray<IVoxelNodeStatProvider*> GVoxelNodeStatProviders;
#endif

#if WITH_EDITOR
class FVoxelNodeStatManager
	: public FVoxelSingleton
	, public IVoxelNodeStatProvider
{
public:
	struct FQueuedStat
	{
		FVoxelGraphNodeRef NodeRef;
		double Duration = 0.;
		int64 Count = 0;
	};
	TQueue<FQueuedStat, EQueueMode::Mpsc> Queue;

	struct FStats
	{
		double Time = 0.;
		int64 NumElements = 0;
	};
	TVoxelMap<TWeakObjectPtr<const UEdGraphNode>, FStats> NodeToStats;

	//~ Begin FVoxelSingleton Interface
	virtual void Initialize() override
	{
		GVoxelNodeStatProviders.Add(this);
	}
	virtual void Tick() override
	{
		VOXEL_FUNCTION_COUNTER();

		FQueuedStat QueuedStat;
		while (Queue.Dequeue(QueuedStat))
		{
			UEdGraphNode* GraphNode = QueuedStat.NodeRef.GetGraphNode_EditorOnly();
			if (!ensure(GraphNode))
			{
				continue;
			}

			FStats& Stats = NodeToStats.FindOrAdd(GraphNode);
			Stats.Time += QueuedStat.Duration;
			Stats.NumElements += QueuedStat.Count;
		}
	}
	//~ End FVoxelSingleton Interface

	//~ Begin IVoxelNodeStatProvider Interface
	virtual void ClearStats() override
	{
		NodeToStats.Empty();
	}
	virtual FText GetToolTip(const UEdGraphNode& Node) override
	{
		const FStats* Stats = NodeToStats.Find(&Node);
		if (!Stats)
		{
			return {};
		}

		return FText::Format(INVTEXT("Computed {0} elements in {1}, for an average of {2} per element"),
			FVoxelUtilities::ConvertToNumberText(Stats->NumElements),
			FVoxelUtilities::ConvertToTimeText(Stats->Time),
			FVoxelUtilities::ConvertToTimeText(Stats->Time / Stats->NumElements));
	}
	virtual FText GetText(const UEdGraphNode& Node) override
	{
		const FStats* Stats = NodeToStats.Find(&Node);
		if (!Stats)
		{
			return {};
		}

		return FText::Format(INVTEXT("{0} x {1} = {2}"),
			FVoxelUtilities::ConvertToTimeText(Stats->Time / Stats->NumElements),
			FVoxelUtilities::ConvertToNumberText(Stats->NumElements),
			FVoxelUtilities::ConvertToTimeText(Stats->Time));
	}
	//~ End IVoxelNodeStatProvider Interface
};
FVoxelNodeStatManager* GVoxelNodeStatManager = MakeVoxelSingleton(FVoxelNodeStatManager);
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
void FVoxelNodeStatScope::RecordStats(const double Duration) const
{
	GVoxelNodeStatManager->Queue.Enqueue(
	{
		Node->GetNodeRef(),
		Duration,
		Count
	});
}
#endif