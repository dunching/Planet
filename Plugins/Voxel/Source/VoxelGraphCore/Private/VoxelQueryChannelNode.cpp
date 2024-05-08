// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelQueryChannelNode.h"
#include "VoxelChannelSubsystem.h"
#include "VoxelGraph.h"
#include "VoxelSurface.h"
#include "Point/VoxelChunkedPointSet.h"
#include "VoxelPositionQueryParameter.h"
#include "EdGraph/EdGraphNode.h"

void FVoxelBrushPriorityQueryParameter::AddMaxPriority(
	FVoxelQueryParameters& Parameters,
	const FName ChannelName,
	const FVoxelBrushPriority Priority)
{
	TVoxelMap<FName, FVoxelBrushPriority> NewChannelToPriority;
	if (const FVoxelBrushPriorityQueryParameter* BrushPriority = Parameters.Find<FVoxelBrushPriorityQueryParameter>())
	{
		NewChannelToPriority = BrushPriority->ChannelToPriority;
	}

	if (FVoxelBrushPriority* PriorityPtr = NewChannelToPriority.Find(ChannelName))
	{
		*PriorityPtr = FMath::Min(*PriorityPtr, Priority);
	}
	else
	{
		NewChannelToPriority.Add(ChannelName, Priority);
	}

	Parameters.Add<FVoxelBrushPriorityQueryParameter>().ChannelToPriority = MoveTemp(NewChannelToPriority);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelNode_QueryChannel::FVoxelNode_QueryChannel()
{
	GetPin(ValuePin).SetType(FVoxelPinType::Make<FVoxelSurface>());
}

#if WITH_EDITOR
void FVoxelNode_QueryChannel::FDefinition::Initialize(UEdGraphNode& EdGraphNode)
{
	GVoxelChannelManager->OnChannelDefinitionsChanged_GameThread.Add(MakeWeakPtrDelegate(this, [this, WeakNode = MakeWeakObjectPtr(&EdGraphNode)]
	{
		UEdGraphNode* GraphNode = WeakNode.Get();
		if (!ensure(GraphNode))
		{
			return;
		}

		GraphNode->ReconstructNode();
		GraphNode->GetTypedOuter<UVoxelGraph>()->ForceRecompile();
	}));
}

bool FVoxelNode_QueryChannel::FDefinition::OnPinDefaultValueChanged(const FName PinName, const FVoxelPinValue& NewDefaultValue)
{
	if (PinName != Node.ChannelPin ||
		!NewDefaultValue.IsValid() ||
		!ensureVoxelSlow(NewDefaultValue.Is<FVoxelChannelName>()))
	{
		return false;
	}

	const FVoxelChannelName ChannelName = NewDefaultValue.Get<FVoxelChannelName>();
	const TOptional<FVoxelChannelDefinition> ChannelDefinition = GVoxelChannelManager->FindChannelDefinition(ChannelName.Name);

	if (!ChannelDefinition ||
		ChannelDefinition->Type == Node.GetPin(Node.ValuePin).GetType())
	{
		return false;
	}

	Node.GetPin(Node.ValuePin).SetType(ChannelDefinition->Type);
	return true;
}
#endif

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_QueryChannel, Value)
{
	if (!Query.GetParameters().Find<FVoxelQueryChannelBoundsQueryParameter>() &&
		!Query.GetParameters().Find<FVoxelPointChunkRefQueryParameter>())
	{
		// Required by FVoxelRuntimeChannel::Get
		FindVoxelQueryParameter(FVoxelPositionQueryParameter, PositionQueryParameter);
	}

	const TValue<FVoxelChannelName> ChannelName = Get(ChannelPin, Query);
	const TValue<int32> MaxPriority = Get(MaxPriorityPin, Query);

	return VOXEL_ON_COMPLETE(ChannelName, MaxPriority)
	{
		const TSharedRef<FVoxelWorldChannelManager> ChannelManager = FVoxelWorldChannelManager::Get(Query.GetInfo(EVoxelQueryInfo::Query).GetWorld());
		const TSharedPtr<FVoxelWorldChannel> WorldChannel = ChannelManager->FindChannel(ChannelName.Name);
		if (!WorldChannel)
		{
			VOXEL_MESSAGE(Error, "{0}: No channel named {1} found. Valid names: {2}",
				this,
				ChannelName.Name,
				ChannelManager->GetValidChannelNames());
			return {};
		}

		if (WorldChannel->Definition.Type != ReturnPinType)
		{
			VOXEL_MESSAGE(Error, "{0}: Invalid value type {1}. Should be the same as channel type {2}",
				this,
				ReturnPinType.ToString(),
				WorldChannel->Definition.Type.ToString());
			return {};
		}

		const TSharedPtr<FVoxelChannelSubsystem> ChannelSubsystem = Query.GetInfo(EVoxelQueryInfo::Query).FindSubsystem<FVoxelChannelSubsystem>();
		if (!ChannelSubsystem)
		{
			ensure(Query.GetInfo(EVoxelQueryInfo::Query).IsDestroyed());
			return {};
		}

		const TSharedRef<FVoxelRuntimeChannel> RuntimeChannel = WorldChannel->GetRuntimeChannel(
			Query.GetQueryToWorld(),
			*ChannelSubsystem->Cache);

		const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
		FVoxelBrushPriorityQueryParameter::AddMaxPriority(
			*Parameters,
			ChannelName.Name,
			FVoxelRuntimeChannel::GetFullPriority(
				MaxPriority,
				Query.GetInfo(EVoxelQueryInfo::Local).GetGraphPath(),
				nullptr,
				Query.GetInfo(EVoxelQueryInfo::Local).GetInstanceName()));

		const FVoxelFutureValue Value = RuntimeChannel->Get(Query.MakeNewQuery(Parameters));
		if (!Value.IsValid())
		{
			return {};
		}
		if (!ensure(Value.GetParentType().CanBeCastedTo(ReturnPinType)))
		{
			return {};
		}
		return Value;
	};
}