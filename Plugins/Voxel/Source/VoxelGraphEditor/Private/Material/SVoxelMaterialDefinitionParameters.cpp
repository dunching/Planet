// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelMaterialDefinitionParameters.h"
#include "VoxelMaterialDefinitionToolkit.h"
#include "VoxelMaterialDefinitionParameterSchemaAction.h"

static constexpr int32 ParametersSectionId = 1;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelMaterialDefinitionParameters::Construct(const FArguments& Args)
{
	check(Args._Definition);

	SVoxelMembers::Construct(
		SVoxelMembers::FArguments()
		.Object(Args._Definition)
		.Toolkit(Args._Toolkit));

	OnParametersChangedHandle = Args._Definition->OnParametersChanged.AddSP(this, &SVoxelMaterialDefinitionParameters::RequestRefresh);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelMaterialDefinitionParameters::CollectStaticSections(TArray<int32>& StaticSectionIds)
{
	StaticSectionIds.Add(ParametersSectionId);
}

FText SVoxelMaterialDefinitionParameters::OnGetSectionTitle(const int32 SectionId)
{
	static const TArray<FText> NodeSectionNames
	{
		INVTEXT(""),
		INVTEXT("Parameters"),
	};

	if (!ensure(NodeSectionNames.IsValidIndex(SectionId)))
	{
		return {};
	}

	return NodeSectionNames[SectionId];
}

TSharedRef<SWidget> SVoxelMaterialDefinitionParameters::OnGetMenuSectionWidget(TSharedRef<SWidget> RowWidget, const int32 SectionId)
{
	switch (SectionId)
	{
	default: check(false);
	case ParametersSectionId: return CreateAddButton(SectionId, INVTEXT("Parameter"), "AddNewParameter");
	}
}

void SVoxelMaterialDefinitionParameters::CollectSortedActions(FVoxelMembersActionsSortHelper& OutActionsList)
{
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	UVoxelMaterialDefinition* Definition = GetObject<UVoxelMaterialDefinition>();
	if (!ensure(Toolkit) ||
		!ensure(Definition))
	{
		return;
	}

	OutActionsList.AddCategoriesSortList(ParametersSectionId, Definition->Categories.Categories);

	for (const FVoxelParameter& Parameter : Definition->Parameters)
	{
		const TSharedRef<FVoxelMaterialDefinitionParameterSchemaAction> NewParameterAction = MakeVoxelShared<FVoxelMaterialDefinitionParameterSchemaAction>(
			FText::FromString(Parameter.Category),
			FText::FromName(Parameter.Name),
			FText::FromString(Parameter.Description),
			1,
			FText::FromString(Parameter.Type.ToString()),
			ParametersSectionId);

		NewParameterAction->WeakToolkit = Toolkit;
		NewParameterAction->WeakMembersWidget = SharedThis(this);
		NewParameterAction->ParameterGuid = Parameter.Guid;

		OutActionsList.AddAction(NewParameterAction, Parameter.Category);
	}
}

void SVoxelMaterialDefinitionParameters::SelectBaseObject()
{
	if (const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>())
	{
		Toolkit->SelectParameter({}, false, false);
	}
}

void SVoxelMaterialDefinitionParameters::GetContextMenuAddOptions(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		INVTEXT("Add new Parameter"),
		FText(),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintEditor.AddNewVariable"),
		FUIAction{
			FExecuteAction::CreateSP(this, &SVoxelMaterialDefinitionParameters::OnAddNewMember, ParametersSectionId)
		});
}

void SVoxelMaterialDefinitionParameters::OnPasteItem(const FString& ImportText, const int32 SectionId)
{
	UVoxelMaterialDefinition* Definition = GetObject<UVoxelMaterialDefinition>();
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	if (!ensure(Definition) ||
		!ensure(Toolkit) ||
		!ensure(SectionId == ParametersSectionId))
	{
		return;
	}

	FStringOutputDevice Errors;

	FVoxelParameter NewParameter;
	FVoxelParameter::StaticStruct()->ImportText(
		ImportText.GetCharArray().GetData(),
		&NewParameter,
		nullptr,
		0,
		&Errors,
		FVoxelParameter::StaticStruct()->GetName());

	if (!Errors.IsEmpty())
	{
		return;
	}

	NewParameter.Guid = FGuid::NewGuid();
	NewParameter.Category = GetPasteCategory();

	{
		const FVoxelTransaction Transaction(Definition, "Paste parameter");
		Definition->Parameters.Add(NewParameter);
	}

	Toolkit->SelectParameter(NewParameter.Guid, true, true);
}

bool SVoxelMaterialDefinitionParameters::CanPasteItem(const FString& ImportText, const int32 SectionId)
{
	if (!ensure(SectionId == ParametersSectionId))
	{
		return false;
	}

	FStringOutputDevice Errors;

	FVoxelParameter Parameter;
	FVoxelParameter::StaticStruct()->ImportText(
		ImportText.GetCharArray().GetData(),
		&Parameter,
		nullptr,
		0,
		&Errors,
		FVoxelParameter::StaticStruct()->GetName());

	return Errors.IsEmpty();
}

void SVoxelMaterialDefinitionParameters::OnAddNewMember(int32 SectionId)
{
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	UVoxelMaterialDefinition* Definition = GetObject<UVoxelMaterialDefinition>();
	if (!ensure(Toolkit) ||
		!ensure(Definition))
	{
		return;
	}

	const FGuid Guid = FGuid::NewGuid();
	{
		const FVoxelTransaction Transaction(Definition, "Create new parameter");

		FVoxelParameter NewParameter;
		NewParameter.Name = "NewParameter";
		NewParameter.Guid = Guid;
		NewParameter.Category = GetPasteCategory();
		NewParameter.Type = FVoxelPinType::Make<float>();
		Definition->Parameters.Add(NewParameter);
	}

	Toolkit->SelectParameter(Guid, true, true);
}

const TArray<FString>& SVoxelMaterialDefinitionParameters::GetCopyPrefixes() const
{
	static const TArray<FString> CopyPrefixes
	{
		"INVALID:",
		"MaterialLayerParameter:",
	};

	return CopyPrefixes;
}

TArray<FString>& SVoxelMaterialDefinitionParameters::GetEditableCategories(int32 SectionId)
{
	UVoxelMaterialDefinition* Definition = GetObject<UVoxelMaterialDefinition>();
	if (!ensure(Definition))
	{
		static TArray<FString> CategoriesList;
		return CategoriesList;
	}

	return Definition->Categories.Categories;
}