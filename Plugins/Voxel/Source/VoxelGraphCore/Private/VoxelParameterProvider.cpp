// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelParameterProvider.h"
#include "VoxelParameterPath.h"

IVoxelParameterProvider* IVoxelParameterProvider::GetSourceProvider()
{
	return this;
}

void IVoxelParameterProvider::AddOnChanged(const FSimpleDelegate& Delegate)
{
	GetSourceProvider()->AddOnChanged(Delegate);
}

TSharedPtr<IVoxelParameterRootView> IVoxelParameterProvider::GetParameterViewImpl(const FVoxelParameterPath& BasePath)
{
	return GetSourceProvider()->GetParameterViewImpl(BasePath);
}

TSharedPtr<IVoxelParameterRootView> IVoxelParameterProvider::GetParameterView()
{
	return GetParameterViewImpl({});
}