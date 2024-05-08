// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMakeValueNode.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeValue, Value)
{
	ensureVoxelSlow(RuntimeValue.IsValid());
	return RuntimeValue;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_MakeValue::GetPromotionTypes(const FVoxelPin& Pin) const
{
	return FVoxelPinTypeSet::All();
}

void FVoxelNode_MakeValue::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	Super::PromotePin(Pin, NewType);
	FixupValue();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNode_MakeValue::PreCompile()
{
	RuntimeValue = FVoxelPinType::MakeRuntimeValue(GetPin(ValuePin).GetType(), Value);
}

void FVoxelNode_MakeValue::PostSerialize()
{
	Super::PostSerialize();
	FixupValue();
}

#if WITH_EDITOR
void FVoxelNode_MakeValue::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FixupValue();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNode_MakeValue::FixupValue()
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelPinType Type = GetPin(ValuePin).GetType().GetExposedType();
	if (Type.IsWildcard())
	{
		Value = {};
		return;
	}

	Value.Fixup(Type, nullptr);
}