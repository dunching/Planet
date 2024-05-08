// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphSchemaAction.generated.h"

USTRUCT()
struct FVoxelGraphSchemaAction : public FEdGraphSchemaAction
{
	GENERATED_BODY();

public:
	using FEdGraphSchemaAction::FEdGraphSchemaAction;

	virtual FName GetTypeId() const final override
	{
		return StaticGetTypeId();
	}

	static FName StaticGetTypeId()
	{
		static const FName TypeId("FVoxelGraphSchemaAction");
		return TypeId;
	}

	virtual void GetIcon(FSlateIcon& Icon, FLinearColor& Color)
	{
		static const FSlateIcon DefaultIcon("EditorStyle", "NoBrush");
		Icon = DefaultIcon;
		Color = FLinearColor::White;
	}
};

USTRUCT()
struct FVoxelGraphSchemaAction_NewComment : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void GetIcon(FSlateIcon& Icon, FLinearColor& Color) override;
};

USTRUCT()
struct FVoxelGraphSchemaAction_Paste : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};