// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelRuntimeParameter.h"
#include "VoxelPointStorage.generated.h"

class FVoxelPointStorageData;

UCLASS()
class UVoxelPointStorage : public UActorComponent
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface

	TSharedRef<FVoxelPointStorageData> GetData();

private:
	FByteBulkData BulkData;
	TSharedPtr<FVoxelPointStorageData> Data;
};

USTRUCT()
struct FVoxelRuntimeParameter_PointStorage : public FVoxelRuntimeParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	TSharedPtr<FVoxelPointStorageData> Data;
};