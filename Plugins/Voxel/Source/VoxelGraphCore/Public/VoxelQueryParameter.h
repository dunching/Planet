// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelQueryParameter.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelQueryParameter
	: public FVoxelVirtualStruct
	, public TSharedFromThis<FVoxelQueryParameter>
{
	GENERATED_BODY()
	DECLARE_VIRTUAL_STRUCT_PARENT(FVoxelQueryParameter, GENERATED_VOXEL_QUERY_PARAMETER_BODY)
};

#define GENERATED_VOXEL_QUERY_PARAMETER_BODY() GENERATED_VIRTUAL_STRUCT_BODY_IMPL(FVoxelQueryParameter)

class VOXELGRAPHCORE_API FVoxelQueryParameters
{
public:
	FVoxelQueryParameters();
	UE_NONCOPYABLE(FVoxelQueryParameters);

	void Add(const TSharedRef<const FVoxelQueryParameter>& QueryParameter);
	void Append(const FVoxelQueryParameters& Other);

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelQueryParameter>::Value>::Type>
	T& Add()
	{
		TSharedRef<T> QueryParameter = MakeVoxelShared<T>();
		this->Add(QueryParameter);
		return *QueryParameter;
	}

	FORCEINLINE const FVoxelQueryParameter* Find(UScriptStruct* Struct) const
	{
		const TSharedPtr<const FVoxelQueryParameter>* QueryParameter = StructToQueryParameter.Find(Struct);
		if (!QueryParameter)
		{
			return nullptr;
		}
		return QueryParameter->Get();
	}
	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelQueryParameter>::Value>::Type>
	FORCEINLINE const T* Find() const
	{
		const FVoxelQueryParameter* QueryParameter = this->Find(StaticStructFast<T>());
		checkVoxelSlow(!QueryParameter || QueryParameter->IsA<T>());
		return static_cast<const T*>(QueryParameter);
	}
	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelQueryParameter>::Value>::Type>
	FORCEINLINE TSharedPtr<const T> FindShared() const
	{
		const T* QueryParameter	 = Find<T>();
		if (!QueryParameter)
		{
			return nullptr;
		}
		return StaticCastSharedRef<const T>(QueryParameter->AsShared());
	}

	TSharedRef<FVoxelQueryParameters> Clone() const;

private:
	TVoxelMap<UScriptStruct*, TSharedPtr<const FVoxelQueryParameter>> StructToQueryParameter;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelLODQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

	int32 LOD = 0;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelGradientStepQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

	float Step = 0.f;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelMinExactDistanceQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

	float MinExactDistance = 0.f;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelQueryChannelBoundsQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

	FVoxelBox Bounds;
};