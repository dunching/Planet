// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelSourceParser.h"
#include "VoxelNode.h"
#if WITH_EDITOR
#include "SourceCodeNavigation.h"
#endif

#if WITH_EDITOR
FVoxelSourceParser* GVoxelSourceParser = MakeVoxelSingleton(FVoxelSourceParser);

void FVoxelSourceParser::Initialize()
{
	FCoreUObjectDelegates::ReloadCompleteDelegate.AddLambda([this](EReloadCompleteReason)
	{
		NodeToPinToTooltip = {};
		FunctionToPropertyToDefault = {};
	});
}

FString FVoxelSourceParser::GetPinTooltip(UScriptStruct* NodeStruct, const FName PinName)
{
	while (
		NodeStruct &&
		NodeStruct != StaticStructFast<FVoxelNode>())
	{
		if (!NodeToPinToTooltip.Contains(NodeStruct))
		{
			BuildPinTooltip(NodeStruct);
		}

		if (const FString* Tooltip = NodeToPinToTooltip.FindChecked(NodeStruct).Find(PinName))
		{
			return *Tooltip;
		}

		NodeStruct = Cast<UScriptStruct>(NodeStruct->GetSuperStruct());
	}
	return PinName.ToString();
}

FString FVoxelSourceParser::GetPropertyDefault(UFunction* Function, const FName InPropertyName)
{
	if (const TMap<FName, FString>* PropertyToDefault = FunctionToPropertyToDefault.Find(Function))
	{
		const FString* Default = PropertyToDefault->Find(InPropertyName);
		if (!ensure(Default))
		{
			return {};
		}
		return *Default;
	}

	VOXEL_FUNCTION_COUNTER();

	TMap<FName, FString>& PropertyToDefault = FunctionToPropertyToDefault.Add(Function);

	FString HeaderPath;
	if (!ensure(FSourceCodeNavigation::FindClassHeaderPath(Function, HeaderPath)))
	{
		return {};
	}

	FString Text;
	if (!ensure(FFileHelper::LoadFileToString(Text, *HeaderPath)))
	{
		return {};
	}

	const FString FunctionName = Function->GetName();

	const auto FindStartIndex = [&](const ESearchDir::Type SearchDir)
	{
		int32 Index = -1;
		do
		{
			Index = Text.Find(
				FunctionName,
				ESearchCase::CaseSensitive,
				SearchDir,
				Index == -1 ? -1 : SearchDir == ESearchDir::FromStart
				? Index + FunctionName.Len()
				: Index - FunctionName.Len());

			if (!ensure(Index != -1))
			{
				return -1;
			}

			if (!Text.IsValidIndex(Index - 1) ||
				Text[Index - 1] != TEXT(' '))
			{
				continue;
			}

			if (!Text.IsValidIndex(Index + FunctionName.Len()) ||
				Text[Index + FunctionName.Len()] != TEXT('('))
			{
				continue;
			}

			break;
		} while (true);

		return Index;
	};

	int32 FunctionStartIndex = FindStartIndex(ESearchDir::FromStart);
	const int32 OtherFunctionStartIndex = FindStartIndex(ESearchDir::FromEnd);
	if (!ensure(FunctionStartIndex != -1) ||
		!ensure(FunctionStartIndex == OtherFunctionStartIndex))
	{
		return {};
	}

	int32 FunctionEndIndex = -1;
	{
		int32 NumScopes = 0;
		for (int32 Index = FunctionStartIndex; Index < Text.Len(); Index++)
		{
			const TCHAR Char = Text[Index];
			if (Char == TEXT('('))
			{
				NumScopes++;

				if (NumScopes == 1)
				{
					FunctionStartIndex = Index;
				}
			}
			else if (Char == TEXT(')'))
			{
				NumScopes--;

				if (NumScopes == 0)
				{
					FunctionEndIndex = Index;
					break;
				}
			}
		}
	}
	if (!ensure(FunctionEndIndex != -1))
	{
		return {};
	}

	ensure(Text[FunctionStartIndex] == TEXT('('));
	ensure(Text[FunctionEndIndex] == TEXT(')'));

	const FString FunctionDeclaration = Text.Mid(FunctionStartIndex + 1, FunctionEndIndex - FunctionStartIndex - 1);

	TArray<FString> Properties;
	{
		int32 NumParenthesisScopes = 0;
		int32 NumBraceScopes = 0;

		FString Property;
		for (const TCHAR Char : FunctionDeclaration)
		{
			Property += Char;

			if (Char == TEXT('('))
			{
				NumParenthesisScopes++;
			}
			else if (Char == TEXT(')'))
			{
				NumParenthesisScopes--;
				ensure(NumParenthesisScopes >= 0);
			}
			else if (Char == TEXT('{'))
			{
				NumBraceScopes++;
			}
			else if (Char == TEXT('}'))
			{
				NumBraceScopes--;
				ensure(NumBraceScopes >= 0);
			}

			if (NumParenthesisScopes > 0 ||
				NumBraceScopes > 0)
			{
				continue;
			}

			if (Char == TEXT(','))
			{
				ensure(Property.RemoveFromEnd(TEXT(",")));
				Properties.Add(Property);
				Property.Reset();
			}
		}

		if (!Property.IsEmpty())
		{
			Properties.Add(Property);
		}
		else
		{
			ensure(Properties.Num() == 0);
		}
	}

	for (FString& Property : Properties)
	{
		Property.TrimStartAndEndInline();

		if (Property.RemoveFromStart(TEXT("UPARAM")))
		{
			Property.TrimStartInline();
			if (!ensure(Property[0] == TEXT('(')))
			{
				continue;
			}

			int32 Index = 1;
			int32 NumScopes = 1;
			while (
				ensure(Property.IsValidIndex(Index)) &&
				NumScopes > 0)
			{
				if (Property[Index] == TEXT(')'))
				{
					NumScopes--;
				}
				Index++;
			}
			Property.RemoveAt(0, Index + 1);
		}
		ensure(!Property.Contains(TEXT("UPARAM"), ESearchCase::CaseSensitive));

		Property.TrimStartInline();
		Property.RemoveFromStart(TEXT("const"));
		Property.TrimStartInline();

		// Remove type
		{
			int32 Index = 0;
			while (
				ensure(Property.IsValidIndex(Index)) &&
				(FChar::IsIdentifier(Property[Index]) || Property[Index] == TEXT(':')))
			{
				Index++;
			}
			Property.RemoveAt(0, Index);
		}

		Property.TrimStartInline();
		Property.RemoveFromStart(TEXT("&"));
		Property.TrimStartInline();

		FString PropertyName;
		{
			int32 Index = 0;
			while (
				Property.IsValidIndex(Index) &&
				FChar::IsIdentifier(Property[Index]))
			{
				PropertyName += Property[Index];
				Index++;
			}
			Property.RemoveAt(0, Index);
		}

		Property.TrimStartInline();
		Property.RemoveFromStart(TEXT("="));
		Property.TrimStartInline();

		const FName PropertyFName(PropertyName);
		ensure(!PropertyToDefault.Contains(PropertyFName));
		PropertyToDefault.Add(PropertyFName, Property);
	}

	TArray<FProperty*> FunctionProperties = GetFunctionProperties(Function).Array();
	FunctionProperties.RemoveSwap(Function->GetReturnProperty());

	ensure(PropertyToDefault.Num() == FunctionProperties.Num());
	for (const FProperty* Property : FunctionProperties)
	{
		ensure(PropertyToDefault.Contains(Property->GetFName()));
	}

	return GetPropertyDefault(Function, InPropertyName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelSourceParser::BuildPinTooltip(UScriptStruct* NodeStruct)
{
	VOXEL_FUNCTION_COUNTER();

	ensure(!NodeToPinToTooltip.Contains(NodeStruct));
	TMap<FName, FString>& PinToTooltip = NodeToPinToTooltip.Add(NodeStruct);

	FString HeaderPath;
	if (!ensure(FSourceCodeNavigation::FindClassHeaderPath(NodeStruct, HeaderPath)))
	{
		return;
	}

	TArray<FString> Lines;
	if (!ensure(FFileHelper::LoadFileToStringArray(Lines, *HeaderPath)))
	{
		return;
	}

	// __LINE__ starts at 1
	Lines.Insert(FString(), 0);

	for (FString& Line : Lines)
	{
		Line.TrimStartAndEndInline();
	}

	TVoxelInstancedStruct<FVoxelNode> NodeInstance(NodeStruct);
	for (const FVoxelPin& Pin : NodeInstance->GetPins())
	{
		if (Pin.Metadata.Line == 0 ||
			Pin.Metadata.Struct != NodeStruct)
		{
			continue;
		}

		if (!ensure(Lines.IsValidIndex(Pin.Metadata.Line)))
		{
			continue;
		}

		int32 FirstLine = Pin.Metadata.Line - 1;
		while (
			Lines.IsValidIndex(FirstLine) &&
			Lines[FirstLine].StartsWith(TEXT("//")))
		{
			FirstLine--;
		}
		FirstLine++;

		if (FirstLine == Pin.Metadata.Line)
		{
			continue;
		}

		FString Comment;
		for (int32 Index = FirstLine; Index < Pin.Metadata.Line; Index++)
		{
			FString Line = Lines[Index];
			Line.RemoveFromStart(TEXT("//"));

			if (!Comment.IsEmpty())
			{
				Comment += TEXT("\n");
			}
			Comment += Line.TrimStartAndEnd();
		}

		ensure(!PinToTooltip.Contains(Pin.Name));
		PinToTooltip.Add(Pin.Name, Comment);
	}
}
#endif