// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelParameter.h"
#include "SchemaActions/VoxelMembersBaseSchemaAction.h"
#include "VoxelMaterialDefinitionParameterSchemaAction.generated.h"

class UVoxelMaterialDefinition;

USTRUCT()
struct FVoxelMaterialDefinitionParameterSchemaAction : public FVoxelMembersBaseSchemaAction
{
	GENERATED_BODY();

public:
	using FVoxelMembersBaseSchemaAction::FVoxelMembersBaseSchemaAction;

	//~ Begin FVoxelGraphMembersBaseSchemaAction Interface
	virtual TSharedRef<SWidget> CreatePaletteWidget(FCreateWidgetForActionData* const InCreateData) const override;
	virtual FReply OnDragged(UObject* Object, const TSharedPtr<FVoxelMembersBaseSchemaAction>& Action, const FPointerEvent& MouseEvent) const override;
	virtual void OnSelected() const override;
	virtual void GetContextMenuActions(FMenuBuilder& MenuBuilder) const override;
	virtual void OnDelete() const override;
	virtual void OnDuplicate() const override;
	virtual bool OnCopy(FString& OutExportText) const override;
	virtual FString GetName() const override;
	virtual void SetName(const FString& Name) const override;
	virtual void SetCategory(const FString& NewCategory) const override;
	//~ End FVoxelGraphMembersBaseSchemaAction Interface

	//~ Begin FEdGraphSchemaAction Interface
	virtual void MovePersistentItemToCategory(const FText& NewCategoryName) override;
	virtual int32 GetReorderIndexInContainer() const override;
	virtual bool ReorderToBeforeAction(TSharedRef<FEdGraphSchemaAction> OtherAction) override;
	virtual FEdGraphSchemaActionDefiningObject GetPersistentItemDefiningObject() const override;
	//~ End FEdGraphSchemaAction Interface

public:
	FVoxelPinType GetPinType() const;
	void SetPinType(const FVoxelPinType& NewPinType) const;

private:
	UVoxelMaterialDefinition* GetDefinition() const;
	FVoxelParameter* GetParameter() const;

public:
	FGuid ParameterGuid;
};