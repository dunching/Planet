// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinValue.h"
#include "VoxelParameterContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoxelParameterFunctionLibrary.generated.h"

class UVoxelParameterContainer;

UCLASS()
class VOXELGRAPHCORE_API UVoxelParameterFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, DisplayName = "Get Voxel Parameter", CustomThunk, Category = "Voxel|Parameters", meta = (CustomStructureParam = "Value", BlueprintInternalUseOnly = "true"))
	static void K2_GetVoxelParameter(
		UVoxelParameterContainer* ParameterContainer,
		FName Name,
		int32& Value)
	{
		unimplemented();
	}

	DECLARE_FUNCTION(execK2_GetVoxelParameter);

	static void GetVoxelParameterImpl(
		UVoxelParameterContainer* ParameterContainer,
		FName Name,
		void* OutData,
		const FProperty* PropertyForTypeCheck);

	static FVoxelPinValue GetVoxelParameter(
		UVoxelParameterContainer* ParameterContainer,
		FName Name);

	template<typename T, typename ReturnType = typename TDecay<decltype(DeclVal<FVoxelPinValue>().Get<T>())>::Type>
	static ReturnType GetVoxelParameterChecked(
		UVoxelParameterContainer* ParameterContainer,
		const FName Name)
	{
		const FVoxelPinValue Value = GetVoxelParameter(ParameterContainer, Name);
		if (!Value.IsValid())
		{
			return {};
		}

		if (!ensure(Value.Is<T>()))
		{
			return {};
		}

		return Value.Get<T>();
	}

public:
	static bool HasVoxelParameter(
		UVoxelParameterContainer* ParameterContainer,
		FName Name);

public:
	UFUNCTION(BlueprintCallable, DisplayName = "Set Voxel Parameter", CustomThunk, Category = "Voxel|Parameters", meta = (AutoCreateRefTerm = "Value", CustomStructureParam = "Value,OutValue", BlueprintInternalUseOnly = "true"))
	static void K2_SetVoxelParameter(
		UVoxelParameterContainer* ParameterContainer,
		FName Name,
		const int32& Value,
		int32& OutValue)
	{
		unimplemented();
	}

	DECLARE_FUNCTION(execK2_SetVoxelParameter);

	static void SetVoxelParameterImpl(
		UVoxelParameterContainer* ParameterContainer,
		FName Name,
		const void* ValuePtr,
		const FProperty* ValueProperty);

	template<typename T, typename = typename TEnableIf<!TIsTObjectPtr<T>::Value && !TIsPointer<T>::Value>::Type>
	static void SetVoxelParameterChecked(
		UVoxelParameterContainer* ParameterContainer,
		const FName Name,
		const T& Value)
	{
		const FVoxelPinValue PinValue = GetVoxelParameter(ParameterContainer, Name);
		if (!PinValue.IsValid())
		{
			return;
		}

		if (!ensure(PinValue.Is<T>()))
		{
			return;
		}

		FString Error;
		if (!ParameterContainer->Set(Name, FVoxelPinValue::Make<T>(Value), &Error))
		{
			VOXEL_MESSAGE(Error, "{0}", Error);
		}
	}
};