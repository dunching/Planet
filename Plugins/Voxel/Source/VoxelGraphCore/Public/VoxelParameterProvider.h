// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelParameterProvider.generated.h"

struct FVoxelParameterPath;
class IVoxelParameterRootView;

UINTERFACE()
class VOXELGRAPHCORE_API UVoxelParameterProvider : public UInterface
{
	GENERATED_BODY()
};

class VOXELGRAPHCORE_API IVoxelParameterProvider : public IInterface
{
	GENERATED_BODY()

public:
	virtual IVoxelParameterProvider* GetSourceProvider();
	virtual void AddOnChanged(const FSimpleDelegate& Delegate);
	virtual TSharedPtr<IVoxelParameterRootView> GetParameterViewImpl(const FVoxelParameterPath& BasePath);

	TSharedPtr<IVoxelParameterRootView> GetParameterView();
};