// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelDynamicValue.h"
#include "Preview/VoxelPreviewHandler.h"
#include "VoxelTexturePreviewHandler.generated.h"

class FDeferredCleanupSlateBrush;

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTexturePreviewHandler : public FVoxelPreviewHandler
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	//~ Begin FVoxelPreviewHandler Interface
	virtual bool IsProcessing() const final override
	{
		return !DynamicValue.IsUpToDate();
	}
	virtual EPreviewType GetPreviewType() const final override
	{
		return EPreviewType::Texture;
	}
	virtual const FSlateBrush* GetBrush_Texture() const final override;
	virtual void Create(const FVoxelPinType& Type) final override;

	virtual void BuildStats(const FAddStat& AddStat) override;
	virtual void UpdateStats(const FVector2D& MousePosition) override;
	//~ End FVoxelPreviewHandler Interface

public:
	virtual FVoxelPinType GetFinalValueType(const FVoxelPinType& Type) const
	{
		return Type;
	}
	virtual FVoxelFutureValue GetFinalValue(
		const FVoxelQuery& Query,
		const FVoxelRuntimePinValue& Value) const
	{
		return Value;
	}

public:
	virtual void GenerateColors(
		const FVoxelRuntimePinValue& Value,
		TVoxelArrayView<FLinearColor> Colors) const VOXEL_PURE_VIRTUAL();

	virtual FString GetValueAt(
		const FVoxelRuntimePinValue& Value,
		int32 Index) const VOXEL_PURE_VIRTUAL({});

	virtual FString GetMinValue() const VOXEL_PURE_VIRTUAL({});
	virtual FString GetMaxValue() const VOXEL_PURE_VIRTUAL({});

private:
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> Texture;

	TSharedPtr<FDeferredCleanupSlateBrush> Brush;
	FVoxelDynamicValue DynamicValue;

	TOptional<FVector2D> QueuedMousePosition;
	FString CurrentValue;

	FVoxelFastCriticalSection CriticalSection;
	FVoxelRuntimePinValue LastValue_RequiresLock;
};