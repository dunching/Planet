﻿// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"

class VOXELCORE_API FVoxelSingleton : public TSharedFromThis<FVoxelSingleton>
{
public:
	FVoxelSingleton();
	virtual ~FVoxelSingleton();
	UE_NONCOPYABLE(FVoxelSingleton);

	virtual void Initialize() {}
	virtual void Tick() {}
	virtual void AddReferencedObjects(FReferenceCollector& Collector) {}
	virtual bool IsEditorOnly() const { return false; }
};

class VOXELCORE_API FVoxelEditorSingleton : public FVoxelSingleton
{
public:
	using FVoxelSingleton::FVoxelSingleton;

	virtual bool IsEditorOnly() const final override
	{
		return true;
	}
};

#define MakeVoxelSingleton(Type) \
	[] \
	{ \
		checkStatic(TIsDerivedFrom<Type, FVoxelSingleton>::Value); \
		VOXEL_FUNCTION_COUNTER(); \
		return new Type(); \
	}()