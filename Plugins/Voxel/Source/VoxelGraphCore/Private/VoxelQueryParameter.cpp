// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelQueryParameter.h"

FVoxelQueryParameters::FVoxelQueryParameters()
{
	StructToQueryParameter.Reserve(8);
}

void FVoxelQueryParameters::Add(const TSharedRef<const FVoxelQueryParameter>& QueryParameter)
{
	UScriptStruct* Struct = QueryParameter->GetStruct();
	checkVoxelSlow(Struct->GetSuperStruct() == FVoxelQueryParameter::StaticStruct());
	StructToQueryParameter.Add(Struct, QueryParameter);
}

void FVoxelQueryParameters::Append(const FVoxelQueryParameters& Other)
{
	for (auto& It : Other.StructToQueryParameter)
	{
		Add(It.Value.ToSharedRef());
	}
}

TSharedRef<FVoxelQueryParameters> FVoxelQueryParameters::Clone() const
{
	const TSharedRef<FVoxelQueryParameters> Result = MakeVoxelShared<FVoxelQueryParameters>();
	Result->StructToQueryParameter = StructToQueryParameter;
	return Result;
}