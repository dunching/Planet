// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelParameterPath.h"

bool FVoxelParameterPath::Serialize(FArchive& Ar)
{
	Ar << Guids;
	return true;
}

bool FVoxelParameterPath::ExportTextItem(
	FString& ValueStr,
	const FVoxelParameterPath& DefaultValue,
	UObject* Parent,
	int32 PortFlags,
	UObject* ExportRootScope) const
{
	if (Guids.Num() == 0)
	{
		ValueStr += "None";
		return true;
	}

	for (const FGuid& Guid : Guids)
	{
		ValueStr += Guid.ToString(EGuidFormats::Digits);
		ValueStr += ".";
	}
	ensure(ValueStr.RemoveFromEnd("."));

	return true;
}

bool FVoxelParameterPath::ImportTextItem(
	const TCHAR*& Buffer,
	int32 PortFlags,
	UObject* Parent,
	FOutputDevice* ErrorText,
	FArchive* InSerializingArchive)
{
	FString ImportedString;
	{
		const TCHAR* Result = FPropertyHelpers::ReadToken(Buffer, ImportedString, true);
		if (!Result)
		{
			return false;
		}
		Buffer = Result;
	}

	if (ImportedString == "None")
	{
		Guids = {};
		return true;
	}

	Guids.Reset();

	TArray<FString> GuidStrings;
	ImportedString.ParseIntoArray(GuidStrings, TEXT("."));

	for (const FString& GuidString : GuidStrings)
	{
		FGuid Guid;
		if (!ensure(FGuid::ParseExact(GuidString, EGuidFormats::Digits, Guid)))
		{
			return false;
		}

		Guids.Add(Guid);
	}

	return true;
}

FVoxelParameterPath FVoxelParameterPath::Make(const FGuid& Guid)
{
	return { { Guid } };
}

FString FVoxelParameterPath::ToString() const
{
	FString Result;
	for (const FGuid& Guid : Guids)
	{
		if (!Result.IsEmpty())
		{
			Result += ".";
		}

		Result += Guid.ToString();
	}
	return Result;
}

FVoxelParameterPath FVoxelParameterPath::GetParent() const
{
	FVoxelParameterPath Result = *this;
	if (ensure(Result.Guids.Num() > 0))
	{
		Result.Guids.Pop(false);
	}
	return Result;
}

FVoxelParameterPath FVoxelParameterPath::MakeChild(const FGuid& Guid) const
{
	ensure(Num() < 32);
	FVoxelParameterPath Result = *this;
	Result.Guids.Add(Guid);
	return Result;
}

FVoxelParameterPath FVoxelParameterPath::MakeChild(const FVoxelParameterPath& Other) const
{
	ensure(Num() < 32);
	FVoxelParameterPath Result = *this;
	Result.Guids.Append(Other.Guids);
	return Result;
}

bool FVoxelParameterPath::StartsWith(const FVoxelParameterPath& Other) const
{
	if (Other.Num() > Num())
	{
		return false;
	}

	for (int32 Index = 0; Index < Other.Num(); Index++)
	{
		if (Guids[Index] != Other.Guids[Index])
		{
			return false;
		}
	}

	return true;
}

FVoxelParameterPath FVoxelParameterPath::MakeRelative(const FVoxelParameterPath& BasePath) const
{
	if (!ensure(StartsWith(BasePath)))
	{
		return {};
	}

	FVoxelParameterPath Result;
	for (int32 Index = BasePath.Num(); Index < Num(); Index++)
	{
		Result.Guids.Add(Guids[Index]);
	}
	return Result;
}