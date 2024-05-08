// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphParameterSelectionCustomization.h"
#include "VoxelGraph.h"
#include "Nodes/VoxelGraphMacroParameterNode.h"

void FVoxelGraphParameterSelectionCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	const TArray<TWeakObjectPtr<UObject>> SelectedObjects = DetailLayout.GetSelectedObjects();
	if (SelectedObjects.Num() != 1 ||
		!ensure(EdGraph.IsValid()))
	{
		return;
	}

	DetailLayout.HideCategory("Config");

	const TSharedRef<IPropertyHandle> ParametersHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraph, Parameters), UVoxelGraph::StaticClass());
	DetailLayout.HideProperty(ParametersHandle);

	uint32 ParametersCount = 0;
	ParametersHandle->GetNumChildren(ParametersCount);

	TSharedPtr<IPropertyHandle> ParameterHandle;
	for (uint32 Index = 0; Index < ParametersCount; Index++)
	{
		const TSharedPtr<IPropertyHandle> ChildParameterHandle = ParametersHandle->GetChildHandle(Index);

		FVoxelGraphParameter ChildParameter = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelGraphParameter>(ChildParameterHandle);
		if (ChildParameter.Guid != TargetParameterGuid)
		{
			continue;
		}

		ensure(!ParameterHandle);
		ParameterHandle = ChildParameterHandle;
	}

	if (!ParameterHandle)
	{
		return;
	}

	const FVoxelGraphParameter Parameter = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelGraphParameter>(ParameterHandle);

	const bool bShowDefaultValue = INLINE_LAMBDA
	{
		switch (Parameter.ParameterType)
		{
		default: ensure(false);
		case EVoxelGraphParameterType::Parameter:
		{
			return true;
		}
		case EVoxelGraphParameterType::Input:
		{
			for (UEdGraphNode* Node : EdGraph->Nodes)
			{
				if (!Node->IsA<UVoxelGraphMacroParameterInputNode>())
				{
					continue;
				}

				const UVoxelGraphMacroParameterInputNode* MacroNode = CastChecked<UVoxelGraphMacroParameterInputNode>(Node);
				if (MacroNode->Guid != TargetParameterGuid ||
					!MacroNode->bExposeDefaultPin)
				{
					continue;
				}

				return false;
			}

			return true;
		}
		case EVoxelGraphParameterType::Output:
		{
			return false;
		}
		case EVoxelGraphParameterType::LocalVariable:
		{
			return false;
		}
		}
	};

	ParameterHandle->SetInstanceMetaData("ShowDefaultValue", bShowDefaultValue ? "true" : "false");

	IDetailCategoryBuilder& ParameterCategory = DetailLayout.EditCategory("Parameter", INVTEXT("Parameter"));
	ParameterCategory.AddProperty(ParameterHandle);

	if (!SelectedNode.IsValid() ||
		!SelectedNode->IsA<UVoxelGraphMacroParameterInputNode>())
	{
		return;
	}

	IDetailCategoryBuilder& NodeCategory = DetailLayout.EditCategory("Node", INVTEXT("Node"));
	NodeCategory.AddExternalObjectProperty(
		{ SelectedNode.Get() },
		GET_MEMBER_NAME_STATIC(UVoxelGraphMacroParameterInputNode, bExposeDefaultPin));
}