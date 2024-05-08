// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphNodeRef.h"

class SVoxelGraphMessages : public SCompoundWidget
{
public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(UVoxelGraph*, Graph);
	};

	void Construct(const FArguments& Args);
	void UpdateNodes();

private:
	class FGraphNode;
	class FPinNode;
	class FMessageNode;

	class INode
	{
	public:
		INode() = default;
		virtual ~INode() = default;

		virtual TSharedRef<SWidget> GetWidget() const = 0;
		virtual TArray<TSharedPtr<INode>> GetChildren() const = 0;
	};
	class FRootNode : public INode
	{
	public:
		const FString Text;
		TMap<TWeakObjectPtr<UVoxelGraph>, TSharedPtr<FGraphNode>> GraphToNode;

		explicit FRootNode(const FString& Text)
			: Text(Text)
		{
		}

		//~ Begin INode Interface
		virtual TSharedRef<SWidget> GetWidget() const override;
		virtual TArray<TSharedPtr<INode>> GetChildren() const override;
		//~ End INode Interface
	};
	class FGraphNode : public INode
	{
	public:
		const TWeakObjectPtr<UVoxelGraph> Graph;
		TMap<FVoxelGraphPinRef, TSharedPtr<FPinNode>> PinRefToNode;
		TMap<TSharedPtr<FTokenizedMessage>, TSharedPtr<FMessageNode>> MessageToNode;

		explicit FGraphNode(const TWeakObjectPtr<UVoxelGraph>& Graph)
			: Graph(Graph)
		{
		}

		//~ Begin INode Interface
		virtual TSharedRef<SWidget> GetWidget() const override;
		virtual TArray<TSharedPtr<INode>> GetChildren() const override;
		//~ End INode Interface
	};
	class FPinNode : public INode
	{
	public:
		const FVoxelGraphPinRef PinRef;
		TMap<TSharedPtr<FTokenizedMessage>, TSharedPtr<FMessageNode>> MessageToNode;

		explicit FPinNode(const FVoxelGraphPinRef& PinRef)
			: PinRef(PinRef)
		{
		}

		//~ Begin INode Interface
		virtual TSharedRef<SWidget> GetWidget() const override;
		virtual TArray<TSharedPtr<INode>> GetChildren() const override;
		//~ End INode Interface
	};
	class FMessageNode : public INode
	{
	public:
		const TSharedRef<FTokenizedMessage> Message;

		explicit FMessageNode(const TSharedRef<FTokenizedMessage>& Message)
			: Message(Message)
		{
		}

		//~ Begin INode Interface
		virtual TSharedRef<SWidget> GetWidget() const override;
		virtual TArray<TSharedPtr<INode>> GetChildren() const override;
		//~ End INode Interface
	};

private:
	using STree = STreeView<TSharedPtr<INode>>;

	TWeakObjectPtr<UVoxelGraph> WeakGraph;
	TSharedPtr<STree> Tree;
	TArray<TSharedPtr<INode>> Nodes;

	TVoxelMap<TWeakPtr<FTokenizedMessage>, TWeakPtr<FTokenizedMessage>> MessageToCanonMessage;
	TMap<FString, TWeakPtr<FTokenizedMessage>> StringToCanonMessage;

	const TSharedRef<FRootNode> CompileNode = MakeVoxelShared<FRootNode>("Compile Errors");
	const TSharedRef<FRootNode> RuntimeNode = MakeVoxelShared<FRootNode>("Runtime Errors");

	TSharedRef<FTokenizedMessage> GetCanonMessage(const TSharedRef<FTokenizedMessage>& Message);
};