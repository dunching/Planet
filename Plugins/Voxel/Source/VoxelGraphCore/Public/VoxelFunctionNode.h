// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelFunctionNode.generated.h"

USTRUCT()
struct FVoxelFunctionNodeMetadataMap
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, FString> MetadataMap;

	// For node comparisons
	friend uint32 GetTypeHash(const FVoxelFunctionNodeMetadataMap& Map)
	{
		return FVoxelUtilities::MurmurHash(Map.MetadataMap.Num());
	}
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHCORE_API FVoxelFunctionNode : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	static TSharedRef<FVoxelFunctionNode> Make(UFunction* InFunction);

#if WITH_EDITOR
	void SetFunction_EditorOnly(UFunction* NewFunction);
#endif

	UFunction* GetFunction() const
	{
		return Function;
	}

	//~ Begin FVoxelNode Interface
	virtual bool IsPureNode() const override
	{
		return true;
	}

#if WITH_EDITOR
	virtual UStruct& GetMetadataContainer() const override;
	virtual FString GetCategory() const override;
	virtual FString GetDisplayName() const override;
#endif

	virtual void PreCompile() override;
	virtual FVoxelComputeValue CompileCompute(FName PinName) const override;
	virtual void PreSerialize() override;
	virtual void PostSerialize() override;
	//~ End FVoxelNode Interface

private:
	UPROPERTY()
	TObjectPtr<UFunction> Function;

	UPROPERTY()
	FName CachedName;

	UPROPERTY()
	TMap<FName, FVoxelFunctionNodeMetadataMap> MetadataMapCache;

	void FixupPins();

private:
	struct FCachedPin
	{
		FName Name;
		bool bIsInput = false;
		FVoxelPinType PropertyType;
		FVoxelPinType PinType;
		FVoxelPinRuntimeId PinId;
	};
	TVoxelArray<FCachedPin> CachedPins;
	TSharedPtr<UVoxelFunctionLibrary::FCachedFunction> CachedFunction;

	friend class UVoxelFunctionLibrary;
};