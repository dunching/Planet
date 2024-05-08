// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelDetailCategoryInterface.h"
#include "VoxelEditorMinimal.h"

class IVoxelDetailCategoryInterfaceCustomNodeBuilder : public IDetailCustomNodeBuilder
{
public:
	FName CategoryName;
	FName CategoryPath;
	TFunction<void(const FVoxelDetailInterface& DetailInterface)> BuildChildren;

	//~ Begin IDetailCustomNodeBuilder Interface
	virtual void GenerateHeaderRowContent(FDetailWidgetRow& NodeRow) override
	{
		NodeRow.NameContent()
		[
			SNew(SVoxelDetailText)
			.Text(FText::FromName(CategoryName))
		];
	}
	virtual void GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder) override
	{
		BuildChildren(ChildrenBuilder);
	}
	virtual FName GetName() const override
	{
		return CategoryPath;
	}
	//~ End IDetailCustomNodeBuilder Interface
};

void FVoxelDetailCategoryInterface::EditCategory(
	const FName CategoryName,
	const FName CategoryPath,
	const TFunction<void(const FVoxelDetailInterface& DetailInterface)> BuildChildren) const
{
	if (IsDetailLayout())
	{
		BuildChildren(GetDetailLayout().EditCategory(CategoryName));
		return;
	}

	if (CategoryName.IsNone())
	{
		BuildChildren(*DetailInterface);
		return;
	}

	const TSharedRef<IVoxelDetailCategoryInterfaceCustomNodeBuilder> CustomBuilder = MakeVoxelShared<IVoxelDetailCategoryInterfaceCustomNodeBuilder>();
	CustomBuilder->CategoryName = CategoryName;
	CustomBuilder->CategoryPath = CategoryPath;
	CustomBuilder->BuildChildren = BuildChildren;
	DetailInterface->AddCustomBuilder(CustomBuilder);
}