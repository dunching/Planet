// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFutureValue.h"
#include "VoxelParameterPath.h"
#include "VoxelGraphInterface.h"
#include "VoxelPinValueInterface.h"
#include "VoxelInlineGraph.generated.h"

class FVoxelDependency;
class FVoxelParameterValues;
class UVoxelParameterContainer;

class VOXELGRAPHCORE_API FVoxelInlineGraphData : public TSharedFromThis<FVoxelInlineGraphData>
{
public:
	const TWeakInterfacePtr<IVoxelParameterProvider> WeakProvider;
	const TSharedRef<FVoxelDependency> Dependency;
	const TOptional<FVoxelParameterPath> ParameterPath;
	// Only set if ParameterPath isn't
	// Should only happen when creating the default InlineGraphData for a macro,
	// or if using MakeValue
	const TSharedPtr<FVoxelParameterValues> ParameterValues;

	static TSharedRef<FVoxelInlineGraphData> Create(
		const TWeakInterfacePtr<IVoxelParameterProvider>& Provider,
		const TOptional<FVoxelParameterPath>& ParameterPath);

	VOXEL_COUNT_INSTANCES();

	TSharedPtr<const FVoxelComputeValue> FindOutput(
		const FVoxelQuery& Query,
		FName OutputName) const;

	TVoxelArray<FName> GetValidOutputNames(const FVoxelQuery& Query) const;
	TWeakObjectPtr<UVoxelGraphInterface> GetWeakGraphInterface(const FVoxelQuery& Query) const;

private:
	mutable FVoxelFastCriticalSection CriticalSection;
	TWeakObjectPtr<UVoxelGraphInterface> WeakGraphInterface_RequiresLock;
	TSharedPtr<int32> DummyPtr_RequiresLock;
	TVoxelMap<FName, TSharedPtr<const FVoxelComputeValue>> OutputNameToCompute_RequiresLock;

	FVoxelInlineGraphData(
		const TWeakInterfacePtr<IVoxelParameterProvider>& WeakProvider,
		const TSharedRef<FVoxelDependency>& Dependency,
		const TOptional<FVoxelParameterPath>& ParameterPath,
		const TSharedPtr<FVoxelParameterValues>& ParameterValues)
		: WeakProvider(WeakProvider)
		, Dependency(Dependency)
		, ParameterPath(ParameterPath)
		, ParameterValues(ParameterValues)
	{
		ensure(ParameterPath.IsSet() != ParameterValues.IsValid());
	}

	void Update();
};

USTRUCT(BlueprintType, meta = (VoxelPinType))
struct VOXELGRAPHCORE_API FVoxelInlineGraph : public FVoxelPinValueInterface
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Voxel", meta = (ProviderClass = "/Script/VoxelGraphCore.VoxelGraphInterface"))
	TObjectPtr<UVoxelParameterContainer> ParameterContainer;

public:
	TSharedPtr<FVoxelInlineGraphData> Data;

	//~ Begin FVoxelPinValueInterface Interface
	virtual void Fixup(UObject* Outer) override;
	virtual void ComputeRuntimeData() override;
	virtual void ComputeExposedData() override;
	//~ End FVoxelPinValueInterface Interface
};