// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

class UVoxelGraph;
struct FVoxelGraphCompileScope;

extern VOXELGRAPHCORE_API FVoxelGraphCompileScope* GVoxelGraphCompileScope;

struct VOXELGRAPHCORE_API FVoxelGraphCompileScope
{
public:
	const UVoxelGraph& Graph;

	explicit FVoxelGraphCompileScope(const UVoxelGraph& Graph);
	~FVoxelGraphCompileScope();

	bool HasError() const
	{
		return bHasError;
	}
	const TArray<TSharedRef<FTokenizedMessage>>& GetMessages() const
	{
		return Messages;
	}

private:
	FVoxelGraphCompileScope* PreviousScope = nullptr;
	bool bHasError = false;
	TArray<TSharedRef<FTokenizedMessage>> Messages;
	TVoxelUniquePtr<FVoxelScopedMessageConsumer> ScopedMessageConsumer;
};