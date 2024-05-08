// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelQuery.h"
#include "VoxelParameterPath.h"
#include "VoxelRuntimePinValue.h"

class IVoxelParameterProvider;

class VOXELGRAPHCORE_API FVoxelParameterValues : public TSharedFromThis<FVoxelParameterValues>
{
public:
	static TSharedRef<FVoxelParameterValues> Create(const TWeakInterfacePtr<IVoxelParameterProvider>& Provider);

	VOXEL_COUNT_INSTANCES();

public:
	FVoxelFutureValue FindParameter(
		const FVoxelPinType& Type,
		const FVoxelParameterPath& Path,
		const FVoxelQuery& Query) const;

private:
	const FName Name;
	const TWeakInterfacePtr<IVoxelParameterProvider> RootProvider;

	mutable FVoxelFastCriticalSection CriticalSection;

	struct FParameterValue
	{
		FVoxelRuntimePinValue Value;
		TSharedPtr<FVoxelDependency> Dependency;
	};
	TVoxelMap<FVoxelParameterPath, FParameterValue> PathToValue_RequiresLock;

	FVoxelParameterValues(
		const FName Name,
		const TWeakInterfacePtr<IVoxelParameterProvider>& RootProvider)
		: Name(Name)
		, RootProvider(RootProvider)
	{
	}

private:
	TVoxelSet<TWeakInterfacePtr<IVoxelParameterProvider>> BoundProviders;

	void BindOnChanged(const TWeakInterfacePtr<IVoxelParameterProvider>& Provider);

private:
	void Update_GameThread();
	void Update_GameThread_RequiresLock();
	FVoxelRuntimePinValue FindParameter_GameThread(
		const FVoxelPinType& Type,
		const FVoxelParameterPath& Path,
		const FVoxelQuery& Query);

	friend class FVoxelParameterValuesManager;
};