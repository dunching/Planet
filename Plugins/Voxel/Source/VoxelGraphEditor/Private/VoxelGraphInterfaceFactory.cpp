// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphInterfaceFactory.h"

#include "VoxelGraph.h"
#include "Interfaces/IMainFrameModule.h"
#include "Widgets/SVoxelNewInstanceAssetDialog.h"

UVoxelGraphInterfaceFactory::UVoxelGraphInterfaceFactory()
{
	SupportedClass = UVoxelGraph::StaticClass();

	bCreateNew = true;
	bEditAfterNew = true;
}

bool UVoxelGraphInterfaceFactory::ConfigureProperties()
{
	const IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	const TSharedPtr<SWindow> ParentWindow = MainFrame.GetParentWindow();

	const TSharedRef<SVoxelNewInstanceAssetDialog> NewGraphDialog =
		SNew(SVoxelNewInstanceAssetDialog, UVoxelGraph::StaticClass())
		.OnGetAssetCategory_Lambda([](const FAssetData& Item)
		{
			FText Category;
			if (!Item.GetTagValue(GET_MEMBER_NAME_CHECKED(UVoxelGraph, Category), Category) &&
				Item.IsAssetLoaded())
			{
				const UVoxelGraph* GraphAsset = Cast<UVoxelGraph>(Item.GetAsset());
				if (GraphAsset != nullptr)
				{
					return FText::FromString(GraphAsset->Category);
				}
			}

			return Category;
		})
		.OnGetAssetDescription_Lambda([](const FAssetData& Item)
		{
			FText Description;
			if (!Item.GetTagValue(GET_MEMBER_NAME_CHECKED(UVoxelGraph, Description), Description) &&
				Item.IsAssetLoaded())
			{
				const UVoxelGraph* GraphAsset = Cast<UVoxelGraph>(Item.GetAsset());
				if (GraphAsset != nullptr)
				{
					return FText::FromString(GraphAsset->Description);
				}
			}

			return Description;
		});
	FSlateApplication::Get().AddModalWindow(NewGraphDialog, ParentWindow);

	if (!NewGraphDialog->GetUserConfirmedSelection())
	{
		return false;
	}

	AssetToCopy = nullptr;

	TOptional<FAssetData> SelectedGraphAsset = NewGraphDialog->GetSelectedAsset();
	if (SelectedGraphAsset.IsSet())
	{
		AssetToCopy = Cast<UVoxelGraph>(SelectedGraphAsset->GetAsset());
		if (AssetToCopy)
		{
			return true;
		}

		const EAppReturnType::Type DialogResult = FMessageDialog::Open(
			EAppMsgType::OkCancel,
			EAppReturnType::Cancel,
			INVTEXT("The selected graph failed to load\nWould you like to create an empty Voxel Graph?"),
			INVTEXT("Create Default?"));

		if (DialogResult == EAppReturnType::Cancel)
		{
			return false;
		}
	}

	return true;
}

UObject* UVoxelGraphInterfaceFactory::FactoryCreateNew(UClass* Class, UObject* InParent, const FName Name, const EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(UVoxelGraph::StaticClass()));

	if (!AssetToCopy)
	{
		return NewObject<UVoxelGraph>(InParent, Class, Name, Flags);
	}

	UVoxelGraph* NewGraph = DuplicateObject<UVoxelGraph>(AssetToCopy, InParent, Name);
	if (!ensure(NewGraph))
	{
		return nullptr;
	}

	NewGraph->Description = {};
	NewGraph->Tooltip = {};
	NewGraph->Category = {};
	return NewGraph;
}