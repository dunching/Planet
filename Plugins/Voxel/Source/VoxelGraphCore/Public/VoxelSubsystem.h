// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelQuery.h"
#include "VoxelSubsystem.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelSubsystem
	: public FVoxelVirtualStruct
	, public TSharedFromThis<FVoxelSubsystem>
	, public TVoxelRuntimeInfo<FVoxelSubsystem>

{
	GENERATED_BODY()
	DECLARE_VIRTUAL_STRUCT_PARENT(FVoxelSubsystem, GENERATED_VOXEL_SUBSYSTEM_BODY)
	VOXEL_COUNT_INSTANCES();

public:
	FORCEINLINE const FVoxelRuntimeInfo& GetRuntimeInfoRef() const
	{
		return *RuntimeInfo;
	}

public:
	virtual bool ShouldCreateSubsystem() const
	{
		return true;
	}

	virtual void Create() {}
	virtual void Destroy() {}
	virtual void Tick() {}
	virtual void AddReferencedObjects(FReferenceCollector& Collector) {}

private:
	TSharedPtr<const FVoxelRuntimeInfo> RuntimeInfo;

	friend FVoxelRuntimeInfoBase;
};

#define GENERATED_VOXEL_SUBSYSTEM_BODY(Name) \
	GENERATED_VIRTUAL_STRUCT_BODY_IMPL(FVoxelSubsystem) \
	void operator=(const Name&) { ensure(false); }