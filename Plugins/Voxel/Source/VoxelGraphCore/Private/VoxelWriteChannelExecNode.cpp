// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelWriteChannelExecNode.h"
#include "VoxelGraph.h"
#include "VoxelSurface.h"
#include "VoxelQueryChannelNode.h"
#include "EdGraph/EdGraphNode.h"

FVoxelWriteChannelExecNode::FVoxelWriteChannelExecNode()
{
	GetPin(ValuePin).SetType(FVoxelPinType::Make<FVoxelSurface>());
}

TVoxelUniquePtr<FVoxelExecNodeRuntime> FVoxelWriteChannelExecNode::CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const
{
	return MakeVoxelUnique<FVoxelWriteChannelExecNodeRuntime>(SharedThis);
}

#if WITH_EDITOR
void FVoxelWriteChannelExecNode::FDefinition::Initialize(UEdGraphNode& EdGraphNode)
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

bool FVoxelWriteChannelExecNode::FDefinition::OnPinDefaultValueChanged(const FName PinName, const FVoxelPinValue& NewDefaultValue)
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelWriteChannelExecNodeRuntime::Create()
{
	VOXEL_FUNCTION_COUNTER();

	const FName ChannelName = GetConstantPin(Node.ChannelPin).Name;
	const int32 Priority = GetConstantPin(Node.PriorityPin);
	const TSharedRef<const FVoxelBounds> Bounds = GetConstantPin(Node.BoundsPin);
	const float Smoothness = GetConstantPin(Node.SmoothnessPin);

	const FVoxelQueryScope Scope(nullptr, &GetContext().Get());

	const TSharedRef<FVoxelWorldChannelManager> ChannelManager = FVoxelWorldChannelManager::Get(GetWorld());
	const TSharedPtr<FVoxelWorldChannel> Channel = ChannelManager->FindChannel(ChannelName);
	if (!Channel)
	{
		VOXEL_MESSAGE(Error, "{0}: No channel named {1} found. Valid names: {2}",
			this,
			ChannelName,
			ChannelManager->GetValidChannelNames());
		return;
	}

	const FString NodeId = GetNodeRef().NodeId.ToString();

	const FVoxelBrushPriority FullPriority = FVoxelRuntimeChannel::GetFullPriority(
		Priority,
		GetGraphPath(),
		&NodeId,
		GetInstanceName());

	FVoxelBox LocalBounds;
	if (Bounds->IsValid())
	{
		// Shouldn't need invalidation
		ensure(Bounds->GetLocalToWorld() == GetRuntimeInfo()->GetLocalToWorld());

		LocalBounds = Bounds->GetBox_NoDependency(GetRuntimeInfo()->GetLocalToWorld());
	}
	else
	{
		LocalBounds = FVoxelBox::Infinite;
	}

	const TSharedRef<FVoxelBrush> Brush = MakeVoxelShared<FVoxelBrush>(
		FName(GetInstanceName()),
		FullPriority,
		LocalBounds,
		GetLocalToWorld(),
		[
			this,
			WeakThis = AsWeak(),
			FullPriority,
			Smoothness = Smoothness,
			Definition = Channel->Definition](const FVoxelQuery& Query) -> FVoxelFutureValue
		{
			const TSharedPtr<FVoxelExecNodeRuntime> This = WeakThis.Pin();
			if (!ensureVoxelSlow(This))
			{
				return {};
			}

			float MinExactDistance = FMath::Max(0.f, Smoothness);
			if (const FVoxelMinExactDistanceQueryParameter* MinExactDistanceQueryParameter = Query.GetParameters().Find<FVoxelMinExactDistanceQueryParameter>())
			{
				MinExactDistance += MinExactDistanceQueryParameter->MinExactDistance;
			}

			const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
			Parameters->Add<FVoxelMinExactDistanceQueryParameter>().MinExactDistance = MinExactDistance;
			FVoxelBrushPriorityQueryParameter::AddMaxPriority(*Parameters, Definition.Name, FullPriority);

			const FVoxelQuery BrushQuery = Query
				.EnterScope(Node)
				.MakeNewQuery(GetContext())
				.MakeNewQuery(Parameters);

			const FVoxelFutureValue Value = Node.GetNodeRuntime().Get(Node.ValuePin, BrushQuery);
			return
				MakeVoxelTask(STATIC_FNAME("Brush"))
				.Dependency(Value)
				.Execute(Definition.Type, [Value, Definition, NodeRef = NodeRef]
				{
					const FVoxelRuntimePinValue SharedValue = Value.GetValue_CheckCompleted();
					if (!SharedValue.CanBeCastedTo(Definition.Type))
					{
						VOXEL_MESSAGE(Error, "{0}: Invalid value type {1}. Should be the same as the type of channel being queried ({2}): {3}",
							NodeRef,
							SharedValue.GetType().ToString(),
							Definition.Name,
							Definition.Type.ToString());
						return FVoxelRuntimePinValue(Definition.Type);
					}
					return SharedValue;
				});
		});

	Channel->AddBrush(Brush, BrushRef);
}

void FVoxelWriteChannelExecNodeRuntime::Destroy()
{
	BrushRef = {};
}