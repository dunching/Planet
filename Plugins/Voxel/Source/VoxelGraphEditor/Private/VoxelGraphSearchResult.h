// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"

struct FVoxelGraphToolkit;
struct FVoxelGraphSearchSettings;

struct FVoxelGraphSearchResult : TSharedFromThis<FVoxelGraphSearchResult>
{
	FString Name;
	const FSlateBrush* Icon = nullptr;
	FLinearColor Color = FLinearColor::White;

	TArray<TSharedPtr<FVoxelGraphSearchResult>> Children;
	TWeakPtr<FVoxelGraphSearchResult> WeakParent;

	virtual ~FVoxelGraphSearchResult() = default;

	TSharedRef<SWidget> GetIcon() const;
	FText GetName() const { return FText::FromString(Name); }

	virtual FText GetSubName() const { return {}; }
	virtual FText GetComment() const { return {}; }
	virtual void OnClick(const TWeakPtr<FVoxelGraphToolkit>& WeakToolkit);
	virtual bool MatchesTokens(const TArray<FString>& Tokens, const FVoxelGraphSearchSettings& Settings, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems) { return false; }

	void AddChild(const TSharedPtr<FVoxelGraphSearchResult>& Child);
	TSharedPtr<FVoxelGraphToolkit> GetToolkit(const TWeakPtr<FVoxelGraphToolkit>& WeakToolkit, const TWeakObjectPtr<UVoxelGraph>& WeakGraph) const;
	static bool StringMatchesSearchTokens(const TArray<FString>& Tokens, FString String, const FVoxelGraphSearchSettings& Settings);
};

struct FVoxelGraphSearchTextResult final : FVoxelGraphSearchResult
{
	explicit FVoxelGraphSearchTextResult(const FString& Text)
	{
		Name = Text;
	}
};

struct FVoxelGraphSearchAssetResult final : FVoxelGraphSearchResult
{
	TWeakObjectPtr<UVoxelGraph> WeakGraph;

	explicit FVoxelGraphSearchAssetResult(const FString& AssetName, UVoxelGraph* Graph)
		: WeakGraph(Graph)
	{
		Name = AssetName;
	}

	virtual void OnClick(const TWeakPtr<FVoxelGraphToolkit>& WeakToolkit) override
	{
		GetToolkit(WeakToolkit, WeakGraph);
	}
};

struct FVoxelGraphSearchGraphResult final : FVoxelGraphSearchResult
{
	TWeakObjectPtr<UVoxelGraph> WeakGraph;

	explicit FVoxelGraphSearchGraphResult(UVoxelGraph* Graph);

	virtual void OnClick(const TWeakPtr<FVoxelGraphToolkit>& WeakToolkit) override;
	virtual bool MatchesTokens(const TArray<FString>& Tokens, const FVoxelGraphSearchSettings& Settings, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems) override;
};

struct FVoxelGraphSearchNodeResult final : FVoxelGraphSearchResult
{
	FString Type;
	FString Comment;
	TWeakObjectPtr<UVoxelGraph> WeakGraph;
	TWeakObjectPtr<UEdGraphNode> WeakNode;

	explicit FVoxelGraphSearchNodeResult(UVoxelGraph* Graph, UEdGraphNode* Node);

	virtual FText GetSubName() const override;
	virtual FText GetComment() const override;
	virtual void OnClick(const TWeakPtr<FVoxelGraphToolkit>& WeakToolkit) override;
	virtual bool MatchesTokens(const TArray<FString>& Tokens, const FVoxelGraphSearchSettings& Settings, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems) override;
};

struct FVoxelGraphSearchPinResult final : FVoxelGraphSearchResult
{
	const FEdGraphPinReference WeakPin;
	FString Type;

	explicit FVoxelGraphSearchPinResult(const UEdGraphPin* Pin);

	virtual FText GetSubName() const override;
	virtual bool MatchesTokens(const TArray<FString>& Tokens, const FVoxelGraphSearchSettings& Settings, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems) override;
};

struct FVoxelGraphSearchParameterResult final : FVoxelGraphSearchResult
{
	const FVoxelGraphParameter& Parameter;
	FString Type;

	explicit FVoxelGraphSearchParameterResult(const FVoxelGraphParameter& Parameter);

	virtual FText GetSubName() const override;
	virtual bool MatchesTokens(const TArray<FString>& Tokens, const FVoxelGraphSearchSettings& Settings, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems) override;
};