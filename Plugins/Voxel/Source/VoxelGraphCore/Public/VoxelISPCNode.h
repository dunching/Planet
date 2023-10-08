// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelISPCNodeHelpers.h"
#include "VoxelISPCNode.generated.h"

USTRUCT(meta = (Abstract))
struct VOXELGRAPHCORE_API FVoxelISPCNode : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	struct FCode
	{
		TArray<FString> Includes;

		void AddInclude(const FString& Include)
		{
			Includes.Add(Include);
		}
	};
	virtual FString GenerateCode(FCode& Code) const VOXEL_PURE_VIRTUAL({});

public:
	//~ Begin FVoxelNode Interface
	virtual bool IsPureNode() const override
	{
		return true;
	}
	virtual void PreCompile() override;
	virtual FVoxelComputeValue CompileCompute(FName PinName) const override;
	//~ End FVoxelNode Interface

private:
	struct FCachedPin
	{
		FName Name;
		bool bIsInput = false;
		FVoxelPinType PinType;
		FVoxelPinRuntimeId PinId;
	};

	FVoxelNodeISPCPtr CachedPtr = nullptr;
	TVoxelArray<FCachedPin> CachedPins;
};