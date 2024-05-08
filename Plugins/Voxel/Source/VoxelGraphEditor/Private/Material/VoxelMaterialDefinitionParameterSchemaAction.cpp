// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMaterialDefinitionParameterSchemaAction.h"
#include "VoxelMaterialDefinitionToolkit.h"
#include "SVoxelMaterialDefinitionParameterPaletteItem.h"
#include "DragDropActions/VoxelMembersBaseDragDropAction.h"

#include "EditorCategoryUtils.h"
#include "Framework/Commands/GenericCommands.h"

TSharedRef<SWidget> FVoxelMaterialDefinitionParameterSchemaAction::CreatePaletteWidget(FCreateWidgetForActionData* const InCreateData) const
{
	return
		SNew(SVoxelMaterialDefinitionParameterPaletteItem, InCreateData)
		.ParametersWidget(GetMembersWidget());
}

FReply FVoxelMaterialDefinitionParameterSchemaAction::OnDragged(UObject* Object, const TSharedPtr<FVoxelMembersBaseSchemaAction>& Action, const FPointerEvent& MouseEvent) const
{
	return FReply::Handled().BeginDragDrop(FVoxelMembersBaseDragDropAction::New(Action));
}

void FVoxelMaterialDefinitionParameterSchemaAction::OnSelected() const
{
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	if (!ensure(Toolkit))
	{
		return;
	}

	Toolkit->SelectParameter(ParameterGuid, false, false);
}

void FVoxelMaterialDefinitionParameterSchemaAction::GetContextMenuActions(FMenuBuilder& MenuBuilder) const
{
	MenuBuilder.BeginSection("BasicOperations");
	{
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, INVTEXT("Rename"), INVTEXT("Renames this parameter") );
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
	}
	MenuBuilder.EndSection();
}

void FVoxelMaterialDefinitionParameterSchemaAction::OnDelete() const
{
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	UVoxelMaterialDefinition* Definition = GetDefinition();
	if (!ensure(Toolkit) ||
		!ensure(Definition))
	{
		return;
	}

	TArray<FVoxelParameter>& Parameters = Definition->Parameters;

	const int32 Index = Parameters.IndexOfByKey(ParameterGuid);
	if (Index == -1)
	{
		return;
	}

	{
		const FVoxelTransaction Transaction(Definition, "Delete parameter");
		Parameters.RemoveAt(Index);
	}

	Toolkit->SelectParameter({}, false, true);
}

void FVoxelMaterialDefinitionParameterSchemaAction::OnDuplicate() const
{
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	UVoxelMaterialDefinition* Definition = GetDefinition();
	if (!ensure(Toolkit) ||
		!ensure(Definition))
	{
		return;
	}

	TArray<FVoxelParameter>& Parameters = Definition->Parameters;

	const FVoxelParameter* Parameter = Parameters.FindByKey(ParameterGuid);
	if (!Parameter)
	{
		return;
	}

	const FGuid NewGuid = FGuid::NewGuid();

	FVoxelParameter NewParameter = *Parameter;
	NewParameter.Guid = NewGuid;

	{
		const FVoxelTransaction Transaction(Definition, "Duplicate parameter");
		Parameters.Add(NewParameter);
	}

	Toolkit->SelectParameter(NewGuid, true, true);
}

bool FVoxelMaterialDefinitionParameterSchemaAction::OnCopy(FString& OutExportText) const
{
	const FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return false;
	}

	FVoxelParameter::StaticStruct()->ExportText(OutExportText, Parameter, Parameter, nullptr, 0, nullptr);

	return true;
}

FString FVoxelMaterialDefinitionParameterSchemaAction::GetName() const
{
	const FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return {};
	}

	return Parameter->Name.ToString();
}

void FVoxelMaterialDefinitionParameterSchemaAction::SetName(const FString& Name) const
{
	FVoxelParameter* Parameter = GetParameter();
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	if (!ensure(Parameter) ||
		!ensure(Toolkit))
	{
		return;
	}

	{
		const FVoxelTransaction Transaction(Toolkit->Asset, "Rename parameter");

		Parameter->Name = *Name;
	}

	Toolkit->SelectParameter(ParameterGuid, false, true);
}

void FVoxelMaterialDefinitionParameterSchemaAction::SetCategory(const FString& NewCategory) const
{
	FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return;
	}

	Parameter->Category = NewCategory;
}

FVoxelPinType FVoxelMaterialDefinitionParameterSchemaAction::GetPinType() const
{
	const FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return FVoxelPinType::Make<float>();
	}

	return Parameter->Type;
}

void FVoxelMaterialDefinitionParameterSchemaAction::SetPinType(const FVoxelPinType& NewPinType) const
{
	FVoxelParameter* Parameter = GetParameter();
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	if (!ensure(Parameter) ||
		!ensure(Toolkit))
	{
		return;
	}

	{
		const FVoxelTransaction Transaction(Toolkit->Asset, "Change parameter type");

		// Materials don't support array
		ensure(!NewPinType.IsBuffer());

		Parameter->Type = NewPinType;
		Parameter->Fixup(nullptr);
	}

	Toolkit->SelectParameter(ParameterGuid, false, true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMaterialDefinitionParameterSchemaAction::MovePersistentItemToCategory(const FText& NewCategoryName)
{
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	UVoxelMaterialDefinition* Definition = GetDefinition();
	FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Toolkit) ||
		!ensure(Definition) ||
		!ensure(Parameter))
	{
		return;
	}

	FString TargetCategory;

	// Need to lookup for original category, since NewCategoryName is returned as display string
	const TArray<FString>& OriginalCategories = Definition->Categories.Categories;
	for (int32 Index = 0; Index < OriginalCategories.Num(); Index++)
	{
		FString ReformattedCategory = FEditorCategoryUtils::GetCategoryDisplayString(OriginalCategories[Index]);
		if (ReformattedCategory == NewCategoryName.ToString())
		{
			TargetCategory = OriginalCategories[Index];
			break;
		}
	}

	if (TargetCategory.IsEmpty())
	{
		TargetCategory = NewCategoryName.ToString();
	}

	{
		const FVoxelTransaction Transaction(Definition, "Move parameter to category");
		Parameter->Category = TargetCategory;
	}

	Toolkit->SelectParameter(ParameterGuid, false, true);
}

int32 FVoxelMaterialDefinitionParameterSchemaAction::GetReorderIndexInContainer() const
{
	UVoxelMaterialDefinition* Definition = GetDefinition();
	if (!Definition)
	{
		return -1;
	}

	return Definition->Parameters.IndexOfByKey(ParameterGuid);
}

bool FVoxelMaterialDefinitionParameterSchemaAction::ReorderToBeforeAction(const TSharedRef<FEdGraphSchemaAction> OtherAction)
{
	if (OtherAction->SectionID != SectionID)
	{
		return false;
	}

	const TSharedPtr<FVoxelMaterialDefinitionParameterSchemaAction> TargetAction = StaticCastSharedRef<FVoxelMaterialDefinitionParameterSchemaAction>(OtherAction);
	if (!ensure(TargetAction))
	{
		return false;
	}

	UVoxelMaterialDefinition* Definition = GetDefinition();
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> TargetToolkit = TargetAction->GetToolkit<FVoxelMaterialDefinitionToolkit>();
	if (!ensure(Definition) ||
		!ensure(Toolkit) ||
		!ensure(TargetToolkit))
	{
		return false;
	}

	if (TargetToolkit->Asset != Toolkit->Asset ||
		TargetAction->ParameterGuid == ParameterGuid)
	{
		return false;
	}

	const FVoxelParameter* ParameterToMove = GetParameter();
	if (!ParameterToMove)
	{
		return false;
	}

	int32 TargetIndex = TargetAction->GetReorderIndexInContainer();
	if (TargetIndex == -1)
	{
		return false;
	}

	const int32 CurrentIndex = GetReorderIndexInContainer();
	if (TargetIndex > CurrentIndex)
	{
		TargetIndex--;
	}

	FVoxelParameter CopiedParameter = *ParameterToMove;

	const FVoxelTransaction Transaction(Toolkit->Asset, "Reorder parameter");

	CopiedParameter.Category = TargetAction->GetParameter()->Category;

	Definition->Parameters.RemoveAt(CurrentIndex);
	Definition->Parameters.Insert(CopiedParameter, TargetIndex);

	return true;
}

FEdGraphSchemaActionDefiningObject FVoxelMaterialDefinitionParameterSchemaAction::GetPersistentItemDefiningObject() const
{
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	if (!Toolkit)
	{
		return nullptr;
	}

	return FEdGraphSchemaActionDefiningObject(Toolkit->Asset);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UVoxelMaterialDefinition* FVoxelMaterialDefinitionParameterSchemaAction::GetDefinition() const
{
	const TSharedPtr<FVoxelMaterialDefinitionToolkit> Toolkit = GetToolkit<FVoxelMaterialDefinitionToolkit>();
	if (!ensure(Toolkit))
	{
		return nullptr;
	}

	UVoxelMaterialDefinition* Definition = Cast<UVoxelMaterialDefinition>(Toolkit->Asset);
	ensure(Definition);
	return Definition;
}

FVoxelParameter* FVoxelMaterialDefinitionParameterSchemaAction::GetParameter() const
{
	UVoxelMaterialDefinition* Definition = GetDefinition();
	if (!Definition)
	{
		return nullptr;
	}

	return Definition->Parameters.FindByKey(ParameterGuid);
}