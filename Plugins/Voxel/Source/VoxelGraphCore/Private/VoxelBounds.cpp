// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelBounds.h"

FVoxelBounds FVoxelBounds::Infinite()
{
	FVoxelBounds Result;
	Result.bIsValid = true;
	Result.Box = FVoxelBox::Infinite;
	Result.LocalToWorld = FVoxelTransformRef::Identity();
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelBounds FVoxelBounds::ExtendLocal(const float LocalAmount) const
{
	FVoxelBounds Result = *this;
	Result.Box = Result.Box.Extend(LocalAmount);
	return Result;
}

FVoxelBounds FVoxelBounds::Extend(
	const float Amount,
	const FVoxelQuery& Query,
	const FVoxelTransformRef& AmountToWorld) const
{
	return ExtendLocal(GetLocalValue(Amount, Query, AmountToWorld));
}

float FVoxelBounds::GetLocalValue(
	const float Value,
	const FVoxelQuery& Query,
	const FVoxelTransformRef& ValueToWorld) const
{
	const FVoxelTransformRef ValueToLocal = ValueToWorld * LocalToWorld.Inverse();
	return Value * ValueToLocal.Get(Query).GetScaleVector().GetAbsMax();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelBox FVoxelBounds::GetBox_NoDependency(const FMatrix& OtherLocalToWorld) const
{
	ensure(bIsValid);
	return Box.TransformBy(
		LocalToWorld.Get_NoDependency() *
		OtherLocalToWorld.Inverse());
}

FVoxelBox FVoxelBounds::GetBox_NoDependency(const FVoxelTransformRef& OtherLocalToWorld) const
{
	ensure(bIsValid);
	return Box.TransformBy((
		LocalToWorld *
		OtherLocalToWorld.Inverse()).Get_NoDependency());
}

FVoxelBox FVoxelBounds::GetBox(const FVoxelQuery& Query, const FVoxelTransformRef& OtherLocalToWorld) const
{
	ensure(bIsValid);
	return Box.TransformBy((
		LocalToWorld *
		OtherLocalToWorld.Inverse()).Get(Query));
}