// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFutureValue.h"
#include "VoxelTransformRef.h"
#include "VoxelRuntimeParameter.h"
#include "VoxelSculptStorage.generated.h"

struct FVoxelSurface;
class FVoxelSculptStorageData;

UCLASS()
class VOXELGRAPHCORE_API UVoxelSculptStorage : public UActorComponent
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface

	void ClearData();
	TSharedRef<FVoxelSculptStorageData> GetData();

private:
	FByteBulkData BulkData;
	TSharedPtr<FVoxelSculptStorageData> Data;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelRuntimeParameter_SculptStorage : public FVoxelRuntimeParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	TSharedPtr<FVoxelSculptStorageData> Data;
	TOptional<FVoxelTransformRef> SurfaceToWorldOverride;

	mutable FVoxelFastCriticalSection CriticalSection;
	mutable float VoxelSize = 0.f;
	mutable TSharedPtr<const TVoxelComputeValue<FVoxelSurface>> Compute_RequiresLock;
};