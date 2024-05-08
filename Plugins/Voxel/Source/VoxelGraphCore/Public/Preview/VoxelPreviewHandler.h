// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinType.h"
#include "VoxelGraphNodeRef.h"
#include "VoxelPreviewHandler.generated.h"

class FVoxelQueryContext;

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelPreviewHandler
	: public FVoxelVirtualStruct
	, public TSharedFromThis<FVoxelPreviewHandler>
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	static const TArray<const FVoxelPreviewHandler*>& GetHandlers();

	virtual bool SupportsType(const FVoxelPinType& Type) const
	{
		return false;
	}

public:
	int32 PreviewSize = 0;
	FVoxelGraphPinRef PinRef;
	TSharedPtr<FVoxelQueryContext> QueryContext;

	enum class EPreviewType
	{
		Texture,
		Viewport
	};
	virtual EPreviewType GetPreviewType() const VOXEL_PURE_VIRTUAL({});

	virtual bool IsProcessing() const { return false; }
	virtual const FSlateBrush* GetBrush_Texture() const { return nullptr; }

	virtual void Create(const FVoxelPinType& Type) VOXEL_PURE_VIRTUAL();

public:
	using FAddStat = TFunctionRef<void(
		const FString& Name,
		const FString& Tooltip,
		const TFunction<FString()>& GetValue)>;

	virtual void BuildStats(const FAddStat& AddStat);
	virtual void UpdateStats(const FVector2D& MousePosition);

private:
	FVector CurrentPosition = FVector::ZeroVector;
};