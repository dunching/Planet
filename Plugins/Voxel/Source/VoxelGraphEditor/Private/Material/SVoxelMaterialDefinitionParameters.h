// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "Widgets/SVoxelMembers.h"
#include "Material/VoxelMaterialDefinition.h"

struct FVoxelMaterialDefinitionToolkit;

class SVoxelMaterialDefinitionParameters : public SVoxelMembers
{
public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(UVoxelMaterialDefinition*, Definition);
		SLATE_ARGUMENT(TSharedPtr<FVoxelMaterialDefinitionToolkit>, Toolkit);
	};

	void Construct(const FArguments& Args);

private:
	//~ Begin SVoxelMembers Interface
	virtual void CollectStaticSections(TArray<int32>& StaticSectionIds) override;
	virtual FText OnGetSectionTitle(int32 SectionId) override;
	virtual TSharedRef<SWidget> OnGetMenuSectionWidget(TSharedRef<SWidget> RowWidget, int32 SectionId) override;
	virtual void CollectSortedActions(FVoxelMembersActionsSortHelper& OutActionsList) override;
	virtual void SelectBaseObject() override;
	virtual void GetContextMenuAddOptions(FMenuBuilder& MenuBuilder) override;
	virtual void OnPasteItem(const FString& ImportText, int32 SectionId) override;
	virtual bool CanPasteItem(const FString& ImportText, int32 SectionId) override;
	virtual void OnAddNewMember(int32 SectionId) override;
	virtual const TArray<FString>& GetCopyPrefixes() const override;

public:
	virtual TArray<FString>& GetEditableCategories(int32 SectionId) override;
	//~ End SVoxelMembers Interface

	FDelegateHandle OnParametersChangedHandle;
};