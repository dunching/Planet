// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Point/VoxelPointHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoxelPointFunctionLibrary.generated.h"

UCLASS()
class VOXELGRAPHCORE_API UVoxelPointFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Voxel|Points", meta = (ExpandEnumAsExecs = ReturnValue))
	static EVoxelSuccess MakePointHandleFromHitResult(const FHitResult& HitResult, FVoxelPointHandle& Handle);

	//UFUNCTION(BlueprintCallable, Category = "Voxel|Points", meta = (ExpandEnumAsExecs = ReturnValue))
	static EVoxelSuccess MakePointHandleFromOverlapResult(const FOverlapResult& OverlapResult, FVoxelPointHandle& Handle);

	UFUNCTION(BlueprintCallable, Category = "Voxel|Points", meta = (ExpandEnumAsExecs = ReturnValue))
	static EVoxelSuccess GetPointTransform(
		const FVoxelPointHandle& Handle,
		FTransform& Transform);

	UFUNCTION(BlueprintCallable, Category = "Voxel|Points")
	static bool HasPointAttribute(
		const FVoxelPointHandle& Handle,
		FName Name);

	template<typename T>
	static bool HasPointAttributeChecked(
		const FVoxelPointHandle& Handle,
		const FName Name)
	{
		FString Error;
		const FVoxelPinValue Value = GetPointAttribute(
			Handle,
			Name,
			&Error);

		if (!Value.IsValid())
		{
			return false;
		}

		return Value.Is<T>();
	}

public:
	// Fast path to hide a point, typically to spawn an actor instead
	// Affects visibility & collision
	// Will not be saved to point storage!
	UFUNCTION(BlueprintCallable, Category = "Voxel|Points")
	static void SetPointVisibility(
		const FVoxelPointHandle& Handle,
		bool bVisible = false);

	static void BulkSetPointVisibility(
		const FVoxelPointChunkRef& ChunkRef,
		TConstVoxelArrayView<FVoxelPointId> PointIds,
		bool bVisible = false);

public:
	UFUNCTION(BlueprintCallable, DisplayName = "Set Point Attribute", CustomThunk, Category = "Voxel|Points", meta = (AutoCreateRefTerm = "Value", CustomStructureParam = "Value", BlueprintInternalUseOnly = "true"))
	static void K2_SetPointAttribute(
		FVoxelPointHandle Handle,
		FName Name,
		FVoxelPinType Type,
		const int32& Value)
	{
		unimplemented();
	}
	DECLARE_FUNCTION(execK2_SetPointAttribute);

	static bool SetPointAttribute(
		const FVoxelPointHandle& Handle,
		FName Name,
		const FVoxelPinType& Type,
		const FVoxelPinValue& Value,
		FString* OutError = nullptr);

	template<typename T, typename = typename TEnableIf<!TIsTObjectPtr<T>::Value && !TIsPointer<T>::Value>::Type>
	static void SetPointAttributeChecked(
		const FVoxelPointHandle& Handle,
		const FName Name,
		const T& Value)
	{
		FString Error;
		ensureMsgf(UVoxelPointFunctionLibrary::SetPointAttribute(
			Handle,
			Name,
			FVoxelPinType::Make<T>().GetBufferType(),
			FVoxelPinValue::Make(Value),
			&Error),
			TEXT("Failed to set attribute: %s"), *Error);
	}

public:
	UFUNCTION(BlueprintCallable, DisplayName = "Get Point Attribute", CustomThunk, Category = "Voxel|Points", meta = (ExpandEnumAsExecs = ReturnValue, AutoCreateRefTerm = "Value", CustomStructureParam = "Value", BlueprintInternalUseOnly = "true"))
	static void K2_GetPointAttribute(
		FVoxelPointHandle Handle,
		FName Name,
		FVoxelPinType Type,
		EVoxelSuccess& ReturnValue,
		int32& Value)
	{
		unimplemented();
	}
	DECLARE_FUNCTION(execK2_GetPointAttribute);

	static FVoxelPinValue GetPointAttribute(
		const FVoxelPointHandle& Handle,
		FName Name,
		FString* OutError = nullptr);

	template<typename T, typename ReturnType = typename TDecay<decltype(DeclVal<FVoxelPinValue>().Get<T>())>::Type>
	static ReturnType GetPointAttributeChecked(
		const FVoxelPointHandle& Handle,
		const FName Name)
	{
		FString Error;
		const FVoxelPinValue Value = GetPointAttribute(
			Handle,
			Name,
			&Error);

		if (!ensureMsgf(Value.IsValid(), TEXT("Failed to get attribute: %s"), *Error))
		{
			return {};
		}

		if (!ensure(Value.CanBeCastedTo<T>()))
		{
			return {};
		}

		return Value.Get<T>();
	}
};