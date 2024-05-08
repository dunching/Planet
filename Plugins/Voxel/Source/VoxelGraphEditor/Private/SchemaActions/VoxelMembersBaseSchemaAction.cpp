// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SchemaActions/VoxelMembersBaseSchemaAction.h"

bool FVoxelMembersBaseSchemaAction::CreateDeletePopups(bool& bOutDeleteNodes, const FString& Title, const FString& ObjectName) const
{
	bOutDeleteNodes = false;
	{
		const EAppReturnType::Type DialogResult = FMessageDialog::Open(
			EAppMsgType::YesNo,
			EAppReturnType::No,
			FText::FromString(ObjectName + " is in use, are you sure you want to delete it?"),
			FText::FromString(Title));

		if (DialogResult == EAppReturnType::No)
		{
			return false;
		}
	}

	{
		const EAppReturnType::Type DialogResult = FMessageDialog::Open(
			EAppMsgType::YesNoCancel,
			EAppReturnType::Cancel,
			FText::FromString("Do you want to delete its references from the graph?"),
			FText::FromString("Delete " + ObjectName + " usages"));

		if (DialogResult == EAppReturnType::Cancel)
		{
			return false;
		}

		bOutDeleteNodes = DialogResult == EAppReturnType::Yes;
	}

	return true;
}