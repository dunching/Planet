// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelRuntimeParameter.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelRuntimeParameter : public FVoxelVirtualStruct
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()
};

class VOXELGRAPHCORE_API FVoxelRuntimeParameters
{
public:
	FVoxelRuntimeParameters() = default;

	void Add(const TSharedRef<const FVoxelRuntimeParameter>& RuntimeParameter)
	{
		ensure(!StructToRuntimeParameter.Contains(RuntimeParameter->GetStruct()));
		StructToRuntimeParameter.Add(RuntimeParameter->GetStruct(), RuntimeParameter);
	}
	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelRuntimeParameter>::Value>::Type>
	T& Add()
	{
		const TSharedRef<T> Parameter = MakeVoxelShared<T>();
		this->Add(Parameter);
		return *Parameter;
	}

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelRuntimeParameter>::Value>::Type>
	bool Remove()
	{
		return StructToRuntimeParameter.Remove(StaticStructFast<T>()) != 0;
	}

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelRuntimeParameter>::Value>::Type>
	TSharedPtr<const T> Find() const
	{
		const TSharedPtr<const FVoxelRuntimeParameter> RuntimeParameter = StructToRuntimeParameter.FindRef(StaticStructFast<T>());
		checkVoxelSlow(!RuntimeParameter || RuntimeParameter->IsA<T>());
		return StaticCastSharedPtr<const T>(RuntimeParameter);
	}

private:
	TVoxelMap<UScriptStruct*, TSharedPtr<const FVoxelRuntimeParameter>> StructToRuntimeParameter;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelRuntimeParameter_DisableCollision : public FVoxelRuntimeParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()
};