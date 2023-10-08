// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelArrayNodes.h"
#include "VoxelBufferUtilities.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ArrayLength, Result)
{
	const TValue<FVoxelBuffer> Values = GetNodeRuntime().Get<FVoxelBuffer>(ValuesPin, Query);

	return VOXEL_ON_COMPLETE(Values)
	{
		return Values->Num();
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_ArrayLength::GetPromotionTypes(const FVoxelPin& Pin) const
{
	return FVoxelPinTypeSet::AllBufferArrays();
}

void FVoxelNode_ArrayLength::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	Pin.SetType(NewType);
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ArrayGetItem, Result)
{
	const TValue<FVoxelBuffer> Values = GetNodeRuntime().Get<FVoxelBuffer>(ValuesPin, Query);
	const TValue<int32> Index = GetNodeRuntime().Get<int32>(IndexPin, Query);

	return VOXEL_ON_COMPLETE(Values, Index)
	{
		const int32 NumValues = Values->Num();
		if (Index < 0 ||
			Index >= NumValues)
		{
			VOXEL_MESSAGE(Error, "{0}: Invalid Index {1}. Values.Num={2}", this, Index, NumValues);
			return {};
		}

		return Values->GetGeneric(Index);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_ArrayGetItem::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ValuesPin)
	{
		return FVoxelPinTypeSet::AllBufferArrays();
	}
	else
	{
		return FVoxelPinTypeSet::AllUniforms();
	}
}

void FVoxelNode_ArrayGetItem::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(ValuesPin).SetType(NewType.GetBufferType().WithBufferArray(true));
	GetPin(ResultPin).SetType(NewType.GetInnerExposedType());
}
#endif