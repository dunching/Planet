// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeImpl.h"
#include "VoxelISPCNodeHelpers.h"
#include "Buffer/VoxelDoubleBuffers.h"
#if WITH_EDITOR
#include "SourceCodeNavigation.h"
#endif

TMap<FName, FVoxelNodeISPCPtr> GVoxelNodeISPCPtrs;

#if WITH_EDITOR
VOXEL_RUN_ON_STARTUP_GAME(VoxelNodeISPC)
{
	TArray<UScriptStruct*> AllStructs = GetDerivedStructs<FVoxelISPCNode>();
	AllStructs.Sort([](const UScriptStruct& A, const UScriptStruct& B)
	{
		return A.GetName() < B.GetName();
	});

	TMap<FString, TArray<UScriptStruct*>> HeaderToStructs;

	for (UScriptStruct* Struct : AllStructs)
	{
		if (Struct->HasMetaData(STATIC_FNAME("Abstract")))
		{
			continue;
		}

		FString StructHeaderPath;
		if (!FSourceCodeNavigation::FindClassHeaderPath(Struct, StructHeaderPath))
		{
			continue;
		}

		HeaderToStructs.FindOrAdd(StructHeaderPath).Add(Struct);
	}

	// If on an artist PC, do not generate any code file and just exit
	const bool bCanModify = FPlatformMisc::IsDebuggerPresent();

	bool bModified = false;
	ON_SCOPE_EXIT
	{
		if (bModified)
		{
			check(bCanModify);
			ensure(false);
			FPlatformMisc::RequestExit(true);
		}
	};

	for (const auto& It : HeaderToStructs)
	{
		const FString Header = It.Key;
		const FString HeaderName = FPaths::GetBaseFilename(Header);
		const TArray<UScriptStruct*>& Structs = It.Value;

		FString CppFile;
		CppFile += "// Copyright Voxel Plugin, Inc. All Rights Reserved.\n";
		CppFile += "\n";
		CppFile += "#include \"VoxelISPCNodeHelpers.h\"\n\n";

		FString ISPCFunctions;
		TSet<FString> ISPCIncludes;

		TSet<UEnum*> UsedEnums;

		for (UScriptStruct* Struct : Structs)
		{
			TVoxelInstancedStruct<FVoxelISPCNode> Node(Struct);

			for (const FVoxelPin& Pin : Node->GetPins())
			{
				const FVoxelPinType Type = Pin.GetType().GetInnerExposedType();
				if (!Type.Is<uint8>())
				{
					continue;
				}

				UEnum* Enum = Type.GetEnum();
				if (!Enum)
				{
					continue;
				}

				UsedEnums.Add(Enum);
			}

			CppFile += "REGISTER_VOXEL_NODE_ISPC(" + HeaderName + ", " + Struct->GetName() + ");\n";

			ISPCFunctions += FVoxelISPCNodeImpl::GenerateFunction(*Node, ISPCIncludes);
		}

		FString ISPCFile;
		ISPCFile += "// Copyright Voxel Plugin, Inc. All Rights Reserved.\n";
		ISPCFile += "\n";
		ISPCFile += "#include \"VoxelISPCNodeHelpers.isph\"\n";

		{
			TArray<FString> Includes = ISPCIncludes.Array();
			Includes.Sort();

			for (const FString& Include : Includes)
			{
				ISPCFile += "#include \"" + Include + "\"\n";
			}
		}
		ISPCFile += "\n";

		{
			TArray<UEnum*> OrderedEnums = UsedEnums.Array();
			OrderedEnums.Sort([](const UEnum& A, const UEnum& B)
			{
				return A.GetName() < B.GetName();
			});

			for (UEnum* EnumType : OrderedEnums)
			{
				ISPCFile += "enum " + EnumType->GetName();
				ISPCFile += "{";
				for (int32 EnumIndex = 0; EnumIndex < EnumType->NumEnums(); EnumIndex++)
				{
					ISPCFile += "\t" + EnumType->GetNameByIndex(EnumIndex).ToString().Replace(TEXT("::"), TEXT("_")) + " = " + LexToString(EnumType->GetValueByIndex(EnumIndex)) + ",\n";
				}
				ISPCFile += "};\n";
			}
		}

		FVoxelISPCNodeImpl::FormatCode(ISPCFunctions);

		ISPCFile += ISPCFunctions;
		ISPCFile.RemoveFromEnd("\n");
		ISPCFile.RemoveFromEnd("\n");

		CppFile.RemoveFromEnd("\n");

		const FString BasePath = FPaths::GetPath(FPaths::ConvertRelativePathToFull(Header));

		const FString ISPCFilePath = BasePath / FPaths::GetBaseFilename(Header) + "_Generated.ispc";
		const FString CppFilePath = BasePath / FPaths::GetBaseFilename(Header) + "_Generated.cpp";

		FString ExistingISPCFile;
		FString ExistingCppFile;
		FFileHelper::LoadFileToString(ExistingISPCFile, *ISPCFilePath);
		FFileHelper::LoadFileToString(ExistingCppFile, *CppFilePath);

		// Normalize line endings
		ExistingISPCFile.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
		ExistingCppFile.ReplaceInline(TEXT("\r\n"), TEXT("\n"));

		if (!ExistingISPCFile.Equals(ISPCFile))
		{
			checkf(bCanModify, TEXT("%s is out of date. There's no debugger attached so we won't generate it. Did you forget to check it in?"), *ISPCFilePath);

			bModified = true;
			IFileManager::Get().Delete(*ISPCFilePath, false, true);
			ensure(FFileHelper::SaveStringToFile(ISPCFile, *ISPCFilePath));
			LOG_VOXEL(Error, "%s written", *ISPCFilePath);
		}
		if (!ExistingCppFile.Equals(CppFile))
		{
			checkf(bCanModify, TEXT("%s is out of date. There's no debugger attached so we won't generate it. Did you forget to check it in?"), *ISPCFilePath);

			bModified = true;
			IFileManager::Get().Delete(*CppFilePath, false, true);
			ensure(FFileHelper::SaveStringToFile(CppFile, *CppFilePath));
			LOG_VOXEL(Error, "%s written", *CppFilePath);
		}
	}
}
#endif

int32 FVoxelISPCNodeImpl::GetRegisterWidth(const FVoxelPinType& Type)
{
	const FVoxelPinType InnerType = Type.GetInnerType();

	if (InnerType.Is<bool>() ||
		InnerType.Is<uint8>() ||
		InnerType.Is<float>() ||
		InnerType.Is<double>() ||
		InnerType.Is<int32>())
	{
		return 1;
	}
	else if (
		InnerType.Is<FVector2D>() ||
		InnerType.Is<FVoxelDoubleVector2D>() ||
		InnerType.Is<FIntPoint>())
	{
		return 2;
	}
	else if (
		InnerType.Is<FVector>() ||
		InnerType.Is<FVoxelDoubleVector>() ||
		InnerType.Is<FIntVector>())
	{
		return 3;
	}
	else if (
		InnerType.Is<FQuat>() ||
		InnerType.Is<FLinearColor>() ||
		InnerType.Is<FVoxelDoubleQuat>() ||
		InnerType.Is<FVoxelDoubleLinearColor>() ||
		InnerType.Is<FIntVector4>())
	{
		return 4;
	}
	else
	{
		check(false);
		return 0;
	}
}

FVoxelPinType FVoxelISPCNodeImpl::GetRegisterType(const FVoxelPinType& Type)
{
	const FVoxelPinType InnerType = Type.GetInnerType();

	if (InnerType.Is<bool>())
	{
		return FVoxelPinType::Make<bool>();
	}
	else if (InnerType.Is<uint8>())
	{
		return FVoxelPinType::Make<uint8>();
	}
	else if (
		InnerType.Is<float>() ||
		InnerType.Is<FVector2D>() ||
		InnerType.Is<FVector>() ||
		InnerType.Is<FQuat>() ||
		InnerType.Is<FLinearColor>())
	{
		return FVoxelPinType::Make<float>();
	}
	else if (InnerType.Is<double>() ||
		InnerType.Is<FVoxelDoubleVector2D>() ||
		InnerType.Is<FVoxelDoubleVector>() ||
		InnerType.Is<FVoxelDoubleQuat>() ||
		InnerType.Is<FVoxelDoubleLinearColor>())
	{
		return FVoxelPinType::Make<double>();
	}
	else if (
		InnerType.Is<int32>() ||
		InnerType.Is<FIntPoint>() ||
		InnerType.Is<FIntVector>() ||
		InnerType.Is<FIntVector4>())
	{
		return FVoxelPinType::Make<int32>();
	}
	else
	{
		check(false);
		return {};
	}
}

void FVoxelISPCNodeImpl::FormatCode(FString& Code)
{
	VOXEL_FUNCTION_COUNTER();

	Code.ReplaceInline(TEXT(";"), TEXT(";\n"));
	Code.ReplaceInline(TEXT("{"), TEXT("\n{\n"));
	Code.ReplaceInline(TEXT("}"), TEXT("\n}\n"));

	{
		TArray<FString> Lines;
		Code.ParseIntoArray(Lines, TEXT("\n"), false);

		for (FString& Line : Lines)
		{
			Line.TrimStartAndEndInline();
		}

		Code = FString::Join(Lines, TEXT("\n"));
		Code.ReplaceInline(TEXT("\n\n}"), TEXT("\n}"));
		Code.ParseIntoArray(Lines, TEXT("\n"), false);

		int32 Padding = 0;
		for (FString& Line : Lines)
		{
			FString Prefix;
			for (int32 Index = 0; Index < Padding; Index++)
			{
				Prefix += "\t";
			}

			if (Line == "{")
			{
				Padding++;
			}
			else if (Line == "}")
			{
				Padding--;
				Prefix.RemoveFromEnd(TEXT("\t"));
			}

			Line = Prefix + Line;
			Line.TrimEndInline();
		}

		Code = FString::Join(Lines, TEXT("\n"));
	}
}

FString FVoxelISPCNodeImpl::GenerateFunction(const FVoxelISPCNode& Node, TSet<FString>& OutIncludes)
{
	VOXEL_FUNCTION_COUNTER();

	FString Result;
	Result += "export void VoxelNode_" + Node.GetStruct()->GetName() + "(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)";
	Result += "{";
	Result += "FOREACH(Index, 0, Num)";
	Result += "{";

	int32 BufferIndex = 0;
	for (const FVoxelPin& Pin : Node.GetPins())
	{
		if (!Pin.bIsInput)
		{
			continue;
		}

		const FVoxelPinType Type = Pin.GetType().GetInnerType();

		if (Type.Is<bool>())
		{
			const int32 Index = BufferIndex++;
			Result += "const bool " + Pin.Name.ToString() + " = LoadBool(Buffers[" + FString::FromInt(Index) + "], Index);";
		}
		else if (Type.Is<uint8>())
		{
			const int32 Index = BufferIndex++;
			Result += "const uint8 " + Pin.Name.ToString() + " = LoadByte(Buffers[" + FString::FromInt(Index) + "], Index);";
		}
		else if (Type.Is<float>())
		{
			const int32 Index = BufferIndex++;
			Result += "const float " + Pin.Name.ToString() + " = LoadFloat(Buffers[" + FString::FromInt(Index) + "], Index);";
		}
		else if (Type.Is<FVector2D>() || Type.Is<FVoxelFloatRange>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			Result += "const float2 " + Pin.Name.ToString() + " = MakeFloat2(" +
				"LoadFloat(Buffers[" + FString::FromInt(IndexX) + "], Index), " +
				"LoadFloat(Buffers[" + FString::FromInt(IndexY) + "], Index));";
		}
		else if (Type.Is<FVector>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			Result += "const float3 " + Pin.Name.ToString() + " = MakeFloat3(" +
				"LoadFloat(Buffers[" + FString::FromInt(IndexX) + "], Index), " +
				"LoadFloat(Buffers[" + FString::FromInt(IndexY) + "], Index), " +
				"LoadFloat(Buffers[" + FString::FromInt(IndexZ) + "], Index));";
		}
		else if (Type.Is<FQuat>() || Type.Is<FLinearColor>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			const int32 IndexW = BufferIndex++;
			Result += "const float4 " + Pin.Name.ToString() + " = MakeFloat4(" +
				"LoadFloat(Buffers[" + FString::FromInt(IndexX) + "], Index), " +
				"LoadFloat(Buffers[" + FString::FromInt(IndexY) + "], Index), " +
				"LoadFloat(Buffers[" + FString::FromInt(IndexZ) + "], Index), " +
				"LoadFloat(Buffers[" + FString::FromInt(IndexW) + "], Index));";
		}
		else if (Type.Is<double>())
		{
			const int32 Index = BufferIndex++;
			Result += "const double " + Pin.Name.ToString() + " = LoadDouble(Buffers[" + FString::FromInt(Index) + "], Index);";
		}
		else if (Type.Is<FVoxelDoubleVector2D>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			Result += "const double2 " + Pin.Name.ToString() + " = MakeDouble2(" +
				"LoadDouble(Buffers[" + FString::FromInt(IndexX) + "], Index), " +
				"LoadDouble(Buffers[" + FString::FromInt(IndexY) + "], Index));";
		}
		else if (Type.Is<FVoxelDoubleVector>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			Result += "const double3 " + Pin.Name.ToString() + " = MakeDouble3(" +
				"LoadDouble(Buffers[" + FString::FromInt(IndexX) + "], Index), " +
				"LoadDouble(Buffers[" + FString::FromInt(IndexY) + "], Index), " +
				"LoadDouble(Buffers[" + FString::FromInt(IndexZ) + "], Index));";
		}
		else if (Type.Is<FVoxelDoubleQuat>() || Type.Is<FVoxelDoubleLinearColor>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			const int32 IndexW = BufferIndex++;
			Result += "const double4 " + Pin.Name.ToString() + " = MakeDouble4(" +
				"LoadDouble(Buffers[" + FString::FromInt(IndexX) + "], Index), " +
				"LoadDouble(Buffers[" + FString::FromInt(IndexY) + "], Index), " +
				"LoadDouble(Buffers[" + FString::FromInt(IndexZ) + "], Index), " +
				"LoadDouble(Buffers[" + FString::FromInt(IndexW) + "], Index));";
		}
		else if (Type.Is<int32>() || Type.Is<FVoxelSeed>())
		{
			const int32 Index = BufferIndex++;
			Result += "const int32 " + Pin.Name.ToString() + " = LoadInt32(Buffers[" + FString::FromInt(Index) + "], Index);";
		}
		else if (Type.Is<FIntPoint>() || Type.Is<FVoxelInt32Range>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			Result += "const int2 " + Pin.Name.ToString() + " = MakeInt2(" +
				"LoadInt32(Buffers[" + FString::FromInt(IndexX) + "], Index), " +
				"LoadInt32(Buffers[" + FString::FromInt(IndexY) + "], Index));";
		}
		else if (Type.Is<FIntVector>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			Result += "const int3 " + Pin.Name.ToString() + " = MakeInt3(" +
				"LoadInt32(Buffers[" + FString::FromInt(IndexX) + "], Index), " +
				"LoadInt32(Buffers[" + FString::FromInt(IndexY) + "], Index), " +
				"LoadInt32(Buffers[" + FString::FromInt(IndexZ) + "], Index));";
		}
		else if (Type.Is<FIntVector4>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			const int32 IndexW = BufferIndex++;
			Result += "const int4 " + Pin.Name.ToString() + " = MakeInt4(" +
				"LoadInt32(Buffers[" + FString::FromInt(IndexX) + "], Index), " +
				"LoadInt32(Buffers[" + FString::FromInt(IndexY) + "], Index), " +
				"LoadInt32(Buffers[" + FString::FromInt(IndexZ) + "], Index), " +
				"LoadInt32(Buffers[" + FString::FromInt(IndexW) + "], Index));";
		}
		else
		{
			check(false);
		}
	}

	for (const FVoxelPin& Pin : Node.GetPins())
	{
		if (Pin.bIsInput)
		{
			continue;
		}

		const FVoxelPinType Type = Pin.GetType().GetInnerType();

		if (Type.Is<bool>())
		{
			Result += "bool " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<float>())
		{
			Result += "float " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<FVector2D>())
		{
			Result += "float2 " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<FVector>())
		{
			Result += "float3 " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<FQuat>() || Type.Is<FLinearColor>())
		{
			Result += "float4 " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<double>())
		{
			Result += "double " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<FVoxelDoubleVector2D>())
		{
			Result += "double2 " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<FVoxelDoubleVector>())
		{
			Result += "double3 " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<FVoxelDoubleQuat>() || Type.Is<FVoxelDoubleLinearColor>())
		{
			Result += "double4 " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<int32>() || Type.Is<FVoxelSeed>())
		{
			Result += "int32 " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<FIntPoint>())
		{
			Result += "int2 " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<FIntVector>())
		{
			Result += "int3 " + Pin.Name.ToString() + ";";
		}
		else if (Type.Is<FIntVector4>())
		{
			Result += "int4 " + Pin.Name.ToString() + ";";
		}
		else
		{
			check(false);
		}
	}

	Result += "\n";

	FVoxelISPCNode::FCode Code;
	FString Body = Node.GenerateCode(Code);
	OutIncludes.Append(Code.Includes);

	for (const FVoxelPin& Pin : Node.GetPins())
	{
		FString Name = Pin.Name.ToString();
		if (!Pin.bIsInput)
		{
			Name.RemoveFromStart(TEXT("Out"));
		}

		Body.TrimStartAndEndInline();
		if (!Body.EndsWith(TEXT(";")))
		{
			Body += ";";
		}

		const FString Id = "{" + Name + "}";
		ensure(Body.Contains(Id));

		Body.ReplaceInline(*Id, *Pin.Name.ToString(), ESearchCase::CaseSensitive);
	}
	Result += Body;

	Result += "\n";

	for (const FVoxelPin& Pin : Node.GetPins())
	{
		if (Pin.bIsInput)
		{
			continue;
		}
		const FVoxelPinType Type = Pin.GetType().GetInnerType();

		if (Type.Is<bool>())
		{
			const int32 Index = BufferIndex++;
			Result += "StoreBool(Buffers[" + FString::FromInt(Index) + "], Index, " + Pin.Name.ToString() + ");";
		}
		else if (Type.Is<uint8>())
		{
			const int32 Index = BufferIndex++;
			Result += "StoreByte(Buffers[" + FString::FromInt(Index) + "], Index, " + Pin.Name.ToString() + ");";
		}
		else if (Type.Is<float>())
		{
			const int32 Index = BufferIndex++;
			Result += "StoreFloat(Buffers[" + FString::FromInt(Index) + "], Index, " + Pin.Name.ToString() + ");";
		}
		else if (Type.Is<FVector2D>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			Result += "StoreFloat(Buffers[" + FString::FromInt(IndexX) + "], Index, " + Pin.Name.ToString() + ".x);";
			Result += "StoreFloat(Buffers[" + FString::FromInt(IndexY) + "], Index, " + Pin.Name.ToString() + ".y);";
		}
		else if (Type.Is<FVector>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			Result += "StoreFloat(Buffers[" + FString::FromInt(IndexX) + "], Index, " + Pin.Name.ToString() + ".x);";
			Result += "StoreFloat(Buffers[" + FString::FromInt(IndexY) + "], Index, " + Pin.Name.ToString() + ".y);";
			Result += "StoreFloat(Buffers[" + FString::FromInt(IndexZ) + "], Index, " + Pin.Name.ToString() + ".z);";
		}
		else if (Type.Is<FQuat>() || Type.Is<FLinearColor>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			const int32 IndexW = BufferIndex++;
			Result += "StoreFloat(Buffers[" + FString::FromInt(IndexX) + "], Index, " + Pin.Name.ToString() + ".x);";
			Result += "StoreFloat(Buffers[" + FString::FromInt(IndexY) + "], Index, " + Pin.Name.ToString() + ".y);";
			Result += "StoreFloat(Buffers[" + FString::FromInt(IndexZ) + "], Index, " + Pin.Name.ToString() + ".z);";
			Result += "StoreFloat(Buffers[" + FString::FromInt(IndexW) + "], Index, " + Pin.Name.ToString() + ".w);";
		}
		else if (Type.Is<double>())
		{
			const int32 Index = BufferIndex++;
			Result += "StoreDouble(Buffers[" + FString::FromInt(Index) + "], Index, " + Pin.Name.ToString() + ");";
		}
		else if (Type.Is<FVoxelDoubleVector2D>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			Result += "StoreDouble(Buffers[" + FString::FromInt(IndexX) + "], Index, " + Pin.Name.ToString() + ".x);";
			Result += "StoreDouble(Buffers[" + FString::FromInt(IndexY) + "], Index, " + Pin.Name.ToString() + ".y);";
		}
		else if (Type.Is<FVoxelDoubleVector>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			Result += "StoreDouble(Buffers[" + FString::FromInt(IndexX) + "], Index, " + Pin.Name.ToString() + ".x);";
			Result += "StoreDouble(Buffers[" + FString::FromInt(IndexY) + "], Index, " + Pin.Name.ToString() + ".y);";
			Result += "StoreDouble(Buffers[" + FString::FromInt(IndexZ) + "], Index, " + Pin.Name.ToString() + ".z);";
		}
		else if (Type.Is<FVoxelDoubleQuat>() || Type.Is<FVoxelDoubleLinearColor>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			const int32 IndexW = BufferIndex++;
			Result += "StoreDouble(Buffers[" + FString::FromInt(IndexX) + "], Index, " + Pin.Name.ToString() + ".x);";
			Result += "StoreDouble(Buffers[" + FString::FromInt(IndexY) + "], Index, " + Pin.Name.ToString() + ".y);";
			Result += "StoreDouble(Buffers[" + FString::FromInt(IndexZ) + "], Index, " + Pin.Name.ToString() + ".z);";
			Result += "StoreDouble(Buffers[" + FString::FromInt(IndexW) + "], Index, " + Pin.Name.ToString() + ".w);";
		}
		else if (Type.Is<int32>() || Type.Is<FVoxelSeed>())
		{
			const int32 Index = BufferIndex++;
			Result += "StoreInt32(Buffers[" + FString::FromInt(Index) + "], Index, " + Pin.Name.ToString() + ");";
		}
		else if (Type.Is<FIntPoint>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			Result += "StoreInt32(Buffers[" + FString::FromInt(IndexX) + "], Index, " + Pin.Name.ToString() + ".x);";
			Result += "StoreInt32(Buffers[" + FString::FromInt(IndexY) + "], Index, " + Pin.Name.ToString() + ".y);";
		}
		else if (Type.Is<FIntVector>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			Result += "StoreInt32(Buffers[" + FString::FromInt(IndexX) + "], Index, " + Pin.Name.ToString() + ".x);";
			Result += "StoreInt32(Buffers[" + FString::FromInt(IndexY) + "], Index, " + Pin.Name.ToString() + ".y);";
			Result += "StoreInt32(Buffers[" + FString::FromInt(IndexZ) + "], Index, " + Pin.Name.ToString() + ".z);";
		}
		else if (Type.Is<FIntVector4>())
		{
			const int32 IndexX = BufferIndex++;
			const int32 IndexY = BufferIndex++;
			const int32 IndexZ = BufferIndex++;
			const int32 IndexW = BufferIndex++;
			Result += "StoreInt32(Buffers[" + FString::FromInt(IndexX) + "], Index, " + Pin.Name.ToString() + ".x);";
			Result += "StoreInt32(Buffers[" + FString::FromInt(IndexY) + "], Index, " + Pin.Name.ToString() + ".y);";
			Result += "StoreInt32(Buffers[" + FString::FromInt(IndexZ) + "], Index, " + Pin.Name.ToString() + ".z);";
			Result += "StoreInt32(Buffers[" + FString::FromInt(IndexW) + "], Index, " + Pin.Name.ToString() + ".w);";
		}
		else
		{
			check(false);
		}
	}

	Result += "}";
	Result += "}";
	Result += "\n";

	return Result;
}