// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelChannel.h"
#include "SVoxelChannelEditor.generated.h"

class SVoxelReadWriteFilePermissionsNotice;

USTRUCT()
struct FVoxelChannelEditorDefinition : public FVoxelChannelExposedDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Config")
	TWeakObjectPtr<UObject> SaveLocation;

	UPROPERTY(EditAnywhere, Category = "Config")
	bool bEditChannel = false;
};

class VOXELGRAPHEDITOR_API SVoxelChannelEditor : public SCompoundWidget
{
public:
	struct FChannelNode
	{
		TMap<FName, TSharedPtr<FChannelNode>> Channels;

		FName Name;
		FName FullPath;
		TWeakObjectPtr<UObject> Owner;
		TWeakPtr<FChannelNode> Parent;
		bool bIsCategory = true;
	};

	DECLARE_DELEGATE_OneParam(FOnChannelSelected, FName);

public:
	VOXEL_SLATE_ARGS()
	{
		FArguments()
			: _MaxHeight(260.f)
		{}

		SLATE_ARGUMENT(float, MaxHeight)
		SLATE_ARGUMENT(FName, SelectedChannel)
		SLATE_EVENT(FOnChannelSelected, OnChannelSelected)
	};

	void Construct(const FArguments& InArgs);

protected:
	//~ Begin SCompoundWidget Interface
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	//~ End SCompoundWidget Interface

private:
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FChannelNode> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void OnGetChildren(TSharedPtr<FChannelNode> Item, TArray<TSharedPtr<FChannelNode>>& OutChildren) const;
	void OnSelectionChanged(TSharedPtr<FChannelNode> ChannelNode, ESelectInfo::Type SelectType);

	FReply OnAddSubChannelClicked(TSharedPtr<FChannelNode> Item);
	FReply OnEditChannelClicked(TSharedPtr<FChannelNode> Item);
	FReply OnDeleteChannelClicked(TSharedPtr<FChannelNode> Item);
	FReply OnBrowseToChannel(TSharedPtr<FChannelNode> Item) const;

private:
	void MapChannels();
	void FilterChannels(bool bOnlySelectedChannel);

public:
	static FString GetChannelFilePath(UObject* Object);

private:
	float MaxHeight = 0.f;
	FName SelectedChannel;
	FOnChannelSelected OnChannelSelectedDelegate;

	FText LookupString;

private:
	TSharedPtr<SBox> ChannelsTreeBox;
	FText AddEditButtonText = INVTEXT("Add new Channel");

	using SChannelsTree = STreeView<TSharedPtr<FChannelNode>>;
	TSharedPtr<SChannelsTree> ChannelsTreeWidget;

	TArray<TSharedPtr<FChannelNode>> ChannelItems;
	TMap<FName, TSharedPtr<FChannelNode>> AllChannels;
};

class SVoxelChannelEditDialog : public SCompoundWidget
{
public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(TSet<FName>, ChannelsList);
		SLATE_ARGUMENT(FVoxelChannelEditorDefinition, ChannelToEdit)
		SLATE_EVENT(SVoxelChannelEditor::FOnChannelSelected, OnChannelSelected)
	};

	void Construct(const FArguments& InArgs);
	static void EditChannel(const TSet<FName>& ChannelsList, const FVoxelChannelEditorDefinition& Channel, const SVoxelChannelEditor::FOnChannelSelected& Delegate);

private:
	FReply OnCreateEditChannelClicked();
	void CreateDetailsView();

private:
	TWeakPtr<SWindow> WeakParentWindow;

	TSharedPtr<IStructureDetailsView> DetailsView;
	TSharedPtr<TStructOnScope<FVoxelChannelEditorDefinition>> StructOnScope;
	TSharedPtr<SVoxelReadWriteFilePermissionsNotice> SettingsCheckoutNotice; 

	TSet<FName> ChannelsList;
	FVoxelChannelEditorDefinition ChannelToEdit;
	SVoxelChannelEditor::FOnChannelSelected OnChannelSelected;

public:
	bool bConfirm = false;
};