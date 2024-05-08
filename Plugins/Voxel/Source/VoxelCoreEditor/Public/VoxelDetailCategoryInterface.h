// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreEditorMinimal.h"
#include "VoxelDetailInterface.h"

class VOXELCOREEDITOR_API FVoxelDetailCategoryInterface
{
public:
	FVoxelDetailCategoryInterface(IDetailLayoutBuilder& DetailLayout)
		: DetailLayout(&DetailLayout)
	{
	}
	FVoxelDetailCategoryInterface(IDetailCategoryBuilder& CategoryBuilder)
		: DetailInterface(CategoryBuilder)
	{
	}
	FVoxelDetailCategoryInterface(IDetailChildrenBuilder& ChildrenBuilder)
		: DetailInterface(ChildrenBuilder)
	{
	}
	FVoxelDetailCategoryInterface(const FVoxelDetailInterface& DetailInterface)
		: DetailInterface(DetailInterface)
	{
	}

	bool IsDetailLayout() const
	{
		if (DetailLayout)
		{
			check(!DetailInterface);
			return true;
		}
		else
		{
			check(DetailInterface);
			return false;
		}
	}

	IDetailLayoutBuilder& GetDetailLayout() const
	{
		check(IsDetailLayout());
		return *DetailLayout;
	}
	FVoxelDetailInterface GetDetailInterface() const
	{
		check(!IsDetailLayout());
		return *DetailInterface;
	}

public:
	// CategoryPath: to save ExpansionState
	void EditCategory(
		FName CategoryName,
		FName CategoryPath,
		TFunction<void(const FVoxelDetailInterface& DetailInterface)> BuildChildren) const;

private:
	IDetailLayoutBuilder* DetailLayout = nullptr;
	TOptional<FVoxelDetailInterface> DetailInterface;
};