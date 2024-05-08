// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "K2Node_VoxelBaseNode.h"

#include "VoxelPinType.h"
#include "Widgets/SVoxelGraphPinTypeSelector.h"

#include "ToolMenu.h"
#include "KismetCompiler.h"
#include "K2Node_MakeArray.h"
#include "K2Node_GetArrayItem.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

void UK2Node_VoxelBaseNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	Super::GetMenuActions(ActionRegistrar);

	if (GetClass()->HasAnyClassFlags(CLASS_Abstract))
	{
		return;
	}

	if (ActionRegistrar.IsOpenForRegistration(GetClass()))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		if (!ensure(NodeSpawner))
		{
			return;
		}

		ActionRegistrar.AddBlueprintAction(GetClass(), NodeSpawner);
	}
}

void UK2Node_VoxelBaseNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	// Stepping through a wildcard property with no link & just a default value is a pain
	// To avoid doing that make an array from the value and get the first element of that array

	for (UEdGraphPin* Pin : GetAllPins())
	{
		if (!IsPinWildcard(*Pin) ||
			Pin->Direction != EGPD_Input ||
			Pin->LinkedTo.Num() > 0)
		{
			continue;
		}

		UK2Node_GetArrayItem* GetArrayItem = CompilerContext.SpawnIntermediateNode<UK2Node_GetArrayItem>(this, SourceGraph);
		{
			FProperty* Property = UK2Node_GetArrayItem::StaticClass()->FindPropertyByName(STATIC_FNAME("bReturnByRefDesired"));
			check(Property);
			CastFieldChecked<FBoolProperty>(Property)->SetPropertyValue_InContainer(GetArrayItem, false);

			GetArrayItem->AllocateDefaultPins();
			GetArrayItem->PostReconstructNode();
			CompilerContext.MessageLog.NotifyIntermediateObjectCreation(GetArrayItem, this);
		}

		{
			UK2Node_MakeArray* MakeArray = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this, SourceGraph);
			MakeArray->AllocateDefaultPins();
			MakeArray->GetOutputPin()->MakeLinkTo(GetArrayItem->GetTargetArrayPin());
			MakeArray->PostReconstructNode();
			CompilerContext.MessageLog.NotifyIntermediateObjectCreation(MakeArray, this);

			// +1 to skip the output pin
			CompilerContext.MovePinLinksToIntermediate(*Pin, *MakeArray->Pins[1]);
		}

		GetArrayItem->GetResultPin()->MakeLinkTo(Pin);
		GetArrayItem->PostReconstructNode();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UK2Node_VoxelBaseNode::AddConvertPinContextAction(UToolMenu* Menu, UGraphNodeContextMenuContext* Context, const FVoxelPinTypeSet& TypeSet) const
{
	FToolMenuSection& Section = Menu->FindOrAddSection("EdGraphSchemaPinActions");
	Section.AddSubMenu(
		"PromotePin",
		INVTEXT("Convert pin"),
		INVTEXT("Convert this pin"),
		MakeLambdaDelegate([=](const FToolMenuContext&) -> TSharedRef<SWidget>
		{
			return
				SNew(SVoxelPinTypeSelector)
				.AllowedTypes(TypeSet)
				.OnTypeChanged_Lambda([=](const FVoxelPinType NewType)
				{
					if (!ensure(Context) ||
						!ensure(Context->Pin))
					{
						return;
					}

					ConstCast(this)->OnPinTypeChange(*ConstCast(Context->Pin), NewType);
				})
				.OnCloseMenu_Lambda([]
				{
					FSlateApplication::Get().ClearAllUserFocus();
				});
		})
	);
}

bool UK2Node_VoxelBaseNode::CanAutoConvert(const UEdGraphPin& Pin, const UEdGraphPin& OtherPin, FString& OutReason) const
{
	// FVoxelPinType::Make will throw ensures with other class types than UScriptStruct and UClass
	if (OtherPin.PinType.PinSubCategoryObject.IsValid() &&
		OtherPin.PinType.PinSubCategoryObject->GetClass() != UScriptStruct::StaticClass() &&
		OtherPin.PinType.PinSubCategoryObject->GetClass() != UClass::StaticClass() &&
		!OtherPin.PinType.PinSubCategoryObject->IsA<UEnum>())
	{
		OutReason = GetNodeTitle(ENodeTitleType::FullTitle).ToString() + " does not support " + UEdGraphSchema_K2::TypeToText(OtherPin.PinType).ToString();
		return false;
	}

	const FVoxelPinType TargetType = FVoxelPinType::MakeFromK2(OtherPin.PinType);
	if (!TargetType.IsValid())
	{
		OutReason = GetNodeTitle(ENodeTitleType::FullTitle).ToString() + " does not support " + UEdGraphSchema_K2::TypeToText(OtherPin.PinType).ToString();
		return false;
	}

	if (TargetType.IsWildcard())
	{
		OutReason = GetNodeTitle(ENodeTitleType::FullTitle).ToString() + " does not support " + UEdGraphSchema_K2::TypeToText(OtherPin.PinType).ToString();
		return false;
	}

	if (!FVoxelPinTypeSet::AllExposed().GetTypes().Contains(TargetType))
	{
		OutReason = GetNodeTitle(ENodeTitleType::FullTitle).ToString() + " does not support " + UEdGraphSchema_K2::TypeToText(OtherPin.PinType).ToString();
		return false;
	}

	return true;
}