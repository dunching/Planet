// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinType.h"
#include "VoxelPinValue.h"
#include "VoxelParameter.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelParameter
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid Guid;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FName Name;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVoxelPinType Type;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FString Category;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FString Description;

	UPROPERTY(EditAnywhere, Category = "Default Value")
	FVoxelPinValue DefaultValue;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TMap<FName, FString> MetaData;
#endif

public:
	bool operator==(const FGuid& OtherGuid) const
	{
		return Guid == OtherGuid;
	}
	bool operator==(const FName OtherName) const
	{
		return Name == OtherName;
	}

public:
	void Fixup(UObject* Outer);
	bool Identical(const FVoxelParameter& Other, bool bCheckDefaultValue) const;

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelParameter>::Value>::Type>
	static void FixupParameterArray(UObject* Outer, TArray<T>& Parameters)
	{
		VOXEL_FUNCTION_COUNTER();

		TSet<FName> Names;
		TSet<FGuid> Guids;

		for (FVoxelParameter& Parameter : Parameters)
		{
			Parameter.Fixup(Outer);

			while (Names.Contains(Parameter.Name) || Parameter.Name.IsNone())
			{
				Parameter.Name.SetNumber(Parameter.Name.GetNumber() + 1);
			}
			while (Guids.Contains(Parameter.Guid) || !Parameter.Guid.IsValid())
			{
				Parameter.Guid = FGuid::NewGuid();
			}

			Names.Add(Parameter.Name);
			Guids.Add(Parameter.Guid);
		}
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelParameterCategories
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FString> Categories;

	void Fixup(const TArray<FString>& CategoriesList);
};