// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSurface.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "Preview/VoxelTexturePreviewHandler.h"
#include "VoxelBasePreviewHandlers.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelScalarPreviewHandler : public FVoxelTexturePreviewHandler
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bNormalize = true;

	FORCEINLINE float GetNormalizedValue(const float Value, const FFloatInterval& MinMax) const
	{
		return bNormalize ? (Value - MinMax.Min) / (MinMax.Max - MinMax.Min) : Value;
	}
};

USTRUCT(DisplayName = "Grayscale")
struct VOXELGRAPHCORE_API FVoxelPreviewHandler_Grayscale : public FVoxelScalarPreviewHandler
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	//~ Begin FVoxelTexturePreviewHandler Interface
	virtual bool SupportsType(const FVoxelPinType& Type) const override
	{
		return Type.Is<FVoxelFloatBuffer>();
	}
	virtual void GenerateColors(
		const FVoxelRuntimePinValue& Value,
		TVoxelArrayView<FLinearColor> Colors) const override;

	virtual FString GetValueAt(const FVoxelRuntimePinValue& Value, int32 Index) const override;
	virtual FString GetMinValue() const override { return LexToSanitizedString(MinValue); }
	virtual FString GetMaxValue() const override { return LexToSanitizedString(MaxValue); }
	//~ End FVoxelTexturePreviewHandler Interface

private:
	mutable float MinValue = 0.f;
	mutable float MaxValue = 0.f;
};

USTRUCT(DisplayName = "Vector2D")
struct VOXELGRAPHCORE_API FVoxelPreviewHandler_Vector2D : public FVoxelScalarPreviewHandler
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	//~ Begin FVoxelTexturePreviewHandler Interface
	virtual bool SupportsType(const FVoxelPinType& Type) const override
	{
		return Type.Is<FVoxelVector2DBuffer>();
	}
	virtual void GenerateColors(
		const FVoxelRuntimePinValue& Value,
		TVoxelArrayView<FLinearColor> Colors) const override;

	virtual FString GetValueAt(const FVoxelRuntimePinValue& Value, int32 Index) const override;
	virtual FString GetMinValue() const override { return MinValue.ToString(); }
	virtual FString GetMaxValue() const override { return MaxValue.ToString(); }
	//~ End FVoxelTexturePreviewHandler Interface

private:
	mutable FVector2f MinValue = FVector2f(ForceInit);
	mutable FVector2f MaxValue = FVector2f(ForceInit);
};

USTRUCT(DisplayName = "Vector")
struct VOXELGRAPHCORE_API FVoxelPreviewHandler_Vector : public FVoxelScalarPreviewHandler
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	//~ Begin FVoxelTexturePreviewHandler Interface
	virtual bool SupportsType(const FVoxelPinType& Type) const override
	{
		return Type.Is<FVoxelVectorBuffer>();
	}
	virtual void GenerateColors(
		const FVoxelRuntimePinValue& Value,
		TVoxelArrayView<FLinearColor> Colors) const override;

	virtual FString GetValueAt(const FVoxelRuntimePinValue& Value, int32 Index) const override;
	virtual FString GetMinValue() const override { return MinValue.ToString(); }
	virtual FString GetMaxValue() const override { return MaxValue.ToString(); }
	//~ End FVoxelTexturePreviewHandler Interface

private:
	mutable FVector3f MinValue = FVector3f(ForceInit);
	mutable FVector3f MaxValue = FVector3f(ForceInit);
};

USTRUCT(DisplayName = "Color")
struct VOXELGRAPHCORE_API FVoxelPreviewHandler_Color : public FVoxelScalarPreviewHandler
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	//~ Begin FVoxelTexturePreviewHandler Interface
	virtual bool SupportsType(const FVoxelPinType& Type) const override
	{
		return Type.Is<FVoxelLinearColorBuffer>();
	}
	virtual void GenerateColors(
		const FVoxelRuntimePinValue& Value,
		TVoxelArrayView<FLinearColor> Colors) const override;

	virtual FString GetValueAt(const FVoxelRuntimePinValue& Value, int32 Index) const override;
	virtual FString GetMinValue() const override { return MinValue.ToString(); }
	virtual FString GetMaxValue() const override { return MaxValue.ToString(); }
	//~ End FVoxelTexturePreviewHandler Interface

private:
	mutable FLinearColor MinValue = FLinearColor(ForceInit);
	mutable FLinearColor MaxValue = FLinearColor(ForceInit);
};

USTRUCT(DisplayName = "Distance Field")
struct VOXELGRAPHCORE_API FVoxelPreviewHandler_DistanceField : public FVoxelTexturePreviewHandler
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	//~ Begin FVoxelTexturePreviewHandler Interface
	virtual bool SupportsType(const FVoxelPinType& Type) const override
	{
		return
			Type.Is<FVoxelSurface>() ||
			Type.Is<FVoxelFloatBuffer>();
	}

public:
	virtual FVoxelPinType GetFinalValueType(const FVoxelPinType& Type) const override
	{
		return FVoxelPinType::Make<FVoxelFloatBuffer>();
	}
	virtual FVoxelFutureValue GetFinalValue(
		const FVoxelQuery& Query,
		const FVoxelRuntimePinValue& Value) const override;

public:
	virtual void GenerateColors(
		const FVoxelRuntimePinValue& Value,
		TVoxelArrayView<FLinearColor> Colors) const override;

	virtual FString GetValueAt(const FVoxelRuntimePinValue& Value, int32 Index) const override;
	virtual FString GetMinValue() const override { return LexToSanitizedString(MinValue); }
	virtual FString GetMaxValue() const override { return LexToSanitizedString(MaxValue); }
	//~ End FVoxelTexturePreviewHandler Interface

private:
	mutable float MinValue = 0.f;
	mutable float MaxValue = 0.f;
};