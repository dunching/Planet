// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelToolkit.h"
#include "VoxelMembersBaseSchemaAction.generated.h"

class SVoxelMembers;
struct FCreateWidgetForActionData;

USTRUCT()
struct VOXELGRAPHEDITOR_API FVoxelMembersBaseSchemaAction : public FEdGraphSchemaAction
{
	GENERATED_BODY();

public:
	using FEdGraphSchemaAction::FEdGraphSchemaAction;

	virtual TSharedRef<SWidget> CreatePaletteWidget(FCreateWidgetForActionData* const InCreateData) const
	{
		return SNullWidget::NullWidget;
	}
	virtual FReply OnDragged(UObject* Object, const TSharedPtr<FVoxelMembersBaseSchemaAction>& Action, const FPointerEvent& MouseEvent) const
	{
		return FReply::Handled();
	}
	virtual void OnActionDoubleClick() const
	{
	}
	virtual void OnSelected() const
	{
	}
	virtual void GetContextMenuActions(FMenuBuilder& MenuBuilder) const
	{
	}
	virtual void OnDelete() const
	{
	}
	virtual void OnDuplicate() const
	{
	}
	virtual bool CanRequestRename() const
	{
		return true;
	}
	virtual bool OnCopy(FString& OutExportText) const
	{
		return false;
	}
	virtual FString GetName() const
	{
		return "";
	}
	virtual void SetName(const FString& Name) const
	{
	}
	virtual void SetCategory(const FString& NewCategory) const
	{
	}
	virtual FString GetSearchString() const
	{
		return GetName();
	}

protected:
	TSharedPtr<FVoxelToolkit> GetToolkit() const
	{
		return WeakToolkit.Pin();
	}
	template<typename Type, typename = typename TEnableIf<TIsDerivedFrom<Type, FVoxelToolkit>::Value>::Type>
	TSharedPtr<Type> GetToolkit() const
	{
		return Cast<Type>(WeakToolkit.Pin());
	}
	TSharedPtr<SVoxelMembers> GetMembersWidget() const
	{
		return WeakMembersWidget.Pin();
	}

	bool CreateDeletePopups(bool& bOutDeleteNodes, const FString& Title, const FString& ObjectName) const;

public:
	TWeakPtr<FVoxelToolkit> WeakToolkit;
	TWeakPtr<SVoxelMembers> WeakMembersWidget;
};