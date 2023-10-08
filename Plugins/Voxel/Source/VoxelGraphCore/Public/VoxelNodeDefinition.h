// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinValue.h"

#if WITH_EDITOR
class VOXELGRAPHCORE_API IVoxelNodeDefinition : public TSharedFromThis<IVoxelNodeDefinition>
{
public:
	enum class ENodeState
	{
		Pin,
		ArrayCategory,
		Category
	};

	struct FNode
	{
		const ENodeState NodeState;
		const FName Name;
		const TArray<FName> Path;
		TArray<TSharedRef<FNode>> Children;

	private:
		explicit FNode(ENodeState NodeState, const FName Name, const TArray<FName>& Path)
			: NodeState(NodeState)
			, Name(Name)
			, Path(Path)
		{
		}

	public:
		FName GetFullPath() const
		{
			if (Path.Num() == 0)
			{
				return {};
			}

			FString ResultPath = Path[0].ToString();
			for (int32 Index = 1; Index < Path.Num(); Index++)
			{
				ResultPath += "|" + Path[Index].ToString();
			}

			return FName(ResultPath);
		}

		static TSharedRef<FNode> MakePin(const FName Name, const TArray<FName>& Path)
		{
			return MakeVoxelShareable(new (GVoxelMemory) FNode(ENodeState::Pin, Name, Path));
		}
		static TSharedRef<FNode> MakeArrayCategory(const FName Name, const TArray<FName>& Path)
		{
			return MakeVoxelShareable(new (GVoxelMemory) FNode(ENodeState::ArrayCategory, Name, Path));
		}
		static TSharedRef<FNode> MakeCategory(const FName Name, const TArray<FName>& Path)
		{
			return MakeVoxelShareable(new (GVoxelMemory) FNode(ENodeState::Category, Name, Path));
		}

		static TArray<FName> MakePath(const FString& RawPath)
		{
			TArray<FString> StringsPath;
			RawPath.ParseIntoArray(StringsPath, TEXT("|"));

			TArray<FName> Result;
			for (const FString& Element : StringsPath)
			{
				Result.Add(FName(Element));
			}

			return Result;
		}
		static TArray<FName> MakePath(const FString& RawPath, const FName ArrayName)
		{
			TArray<FString> StringsPath;
			RawPath.ParseIntoArray(StringsPath, TEXT("|"));
			StringsPath.Add(ArrayName.ToString());

			TArray<FName> Result;
			for (const FString& Element : StringsPath)
			{
				Result.Add(FName(Element));
			}

			return Result;
		}
	};

	IVoxelNodeDefinition() = default;
	virtual ~IVoxelNodeDefinition() = default;

	virtual TSharedPtr<const FNode> GetInputs() const { return nullptr; }
	virtual TSharedPtr<const FNode> GetOutputs() const { return nullptr; }
	virtual bool OverridePinsOrder() const { return false; }

	virtual FString GetAddPinLabel() const { ensure(!CanAddInputPin()); return {}; }
	virtual FString GetAddPinTooltip() const { ensure(!CanAddInputPin()); return {}; }
	virtual FString GetRemovePinTooltip() const { ensure(!CanRemoveInputPin()); return {}; }

	virtual bool CanAddInputPin() const { return false; }
	virtual void AddInputPin() VOXEL_PURE_VIRTUAL();

	virtual bool CanRemoveInputPin() const { return false; }
	virtual void RemoveInputPin() VOXEL_PURE_VIRTUAL();

	virtual bool CanAddToCategory(FName Category) const { return false; }
	virtual void AddToCategory(FName Category) VOXEL_PURE_VIRTUAL();

	virtual bool CanRemoveFromCategory(FName Category) const { return false; }
	virtual void RemoveFromCategory(FName Category) VOXEL_PURE_VIRTUAL();

	virtual bool CanRemoveSelectedPin(FName PinName) const { return false; }
	virtual void RemoveSelectedPin(FName PinName) VOXEL_PURE_VIRTUAL();

	virtual void InsertPinBefore(FName PinName) VOXEL_PURE_VIRTUAL();
	virtual void DuplicatePin(FName PinName) VOXEL_PURE_VIRTUAL();

	virtual bool IsPinVisible(const UEdGraphPin* Pin, const UEdGraphNode* Node) { return true; }
	virtual bool OnPinDefaultValueChanged(FName PinName, const FVoxelPinValue& NewDefaultValue) { return false; }
};
#endif