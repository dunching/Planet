// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGetPreviousOutputNode.h"
#include "VoxelInlineGraph.h"
#include "VoxelFunctionCallNode.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetPreviousOutput, Value)
{
	if (Query.GetContext().GetDepth() >= GVoxelMaxRecursionDepth)
	{
		VOXEL_MESSAGE(Error, "{0}: Max recursion depth reached", this);
		return {};
	}

	const TValue<FName> Name = Get(NamePin, Query);

	return VOXEL_ON_COMPLETE(Name)
	{
		const FVoxelQueryContext& Context = Query.GetContext();

		const int32 GraphArrayIndex = Context.GraphArrayIndex;
		if (GraphArrayIndex == -1 ||
			GraphArrayIndex == 0)
		{
			return Get(DefaultPin, Query);
		}

		const TSharedPtr<const TVoxelArray<TSharedRef<const FVoxelInlineGraphData>>> GraphArray = Context.GraphArray;
		if (!ensure(GraphArray) ||
			!ensure(GraphArray->IsValidIndex(GraphArrayIndex)))
		{
			return {};
		}

		const TSharedRef<const FVoxelInlineGraphData> Data = (*GraphArray)[GraphArrayIndex - 1];
		const TSharedPtr<const FVoxelComputeValue> Output = Data->FindOutput(Query, Name);
		if (!Output)
		{
			VOXEL_MESSAGE(Error, "{0}: No output named {1} on {2}. Valid names: {3}",
				this,
				Name,
				Data->GetWeakGraphInterface(Query),
				Data->GetValidOutputNames(Query));
			return {};
		}

		const FVoxelChildQueryContextKey Key
		{
			// Use a fake node name so that the same cache entry is used for all GetPreviousOutput nodes
			FVoxelGraphNodeRef(GetNodeRef().Graph, FVoxelNodeNames::GetPreviousOutputNodeId),
			Data->ParameterPath.IsSet() ? Query.GetContext().ParameterPath.MakeChild(Data->ParameterPath.GetValue()) : FVoxelParameterPath(),
			Data->ParameterPath.IsSet() ? Query.GetContext().ParameterValues : Data->ParameterValues.ToSharedRef(),
			Context.ComputeInputContext,
			Context.ComputeInputMap,
			GraphArrayIndex - 1,
			GraphArray
		};
		const TSharedRef<FVoxelQueryContext> ChildContext = Query.GetContext().GetChildContext(Key)->EnterScope(GetNodeRef());

		const FVoxelFutureValue Value = (*Output)(Query.MakeNewQuery(ChildContext));
		return VOXEL_ON_COMPLETE(Name, Value)
		{
			if (!Value.GetType().CanBeCastedTo(ReturnPinType))
			{
				VOXEL_MESSAGE(Error, "{0}: Node output pin has type {1}, but output {2} returned type {3}",
					this,
					ReturnPinType.ToString(),
					Name,
					Value.GetType().ToString());
				return {};
			}

			return Value;
		};
	};
}

#if WITH_EDITOR
void FVoxelNode_GetPreviousOutput::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(DefaultPin).SetType(NewType);
	GetPin(ValuePin).SetType(NewType);
}
#endif