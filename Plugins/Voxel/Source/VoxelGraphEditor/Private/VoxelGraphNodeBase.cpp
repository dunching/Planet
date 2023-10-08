// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphNodeBase.h"
#include "VoxelGraphSchemaBase.h"
#include "VoxelGraphToolkit.h"
#include "VoxelEdGraph.h"
#include "Preview/VoxelPreviewHandler.h"

VOXEL_RUN_ON_STARTUP_EDITOR(RegisterOnFocusNode)
{
	FVoxelMessages::OnFocusNode.Add(MakeLambdaDelegate([](const UEdGraphNode& Node)
	{
		if (Node.IsA<UVoxelGraphNodeBase>())
		{
			UVoxelGraphNodeBase::FocusOnNode(ConstCast(&Node));
		}
	}));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TArray<UEdGraphPin*> UVoxelGraphNodeBase::GetInputPins() const
{
	return Pins.FilterByPredicate([&](const UEdGraphPin* Pin) { return Pin->Direction == EGPD_Input; });
}

TArray<UEdGraphPin*> UVoxelGraphNodeBase::GetOutputPins() const
{
	return Pins.FilterByPredicate([&](const UEdGraphPin* Pin) { return Pin->Direction == EGPD_Output; });
}

UEdGraphPin* UVoxelGraphNodeBase::GetInputPin(const int32 Index) const
{
	const TArray<UEdGraphPin*> InputPins = GetInputPins();
	if (!ensure(InputPins.IsValidIndex(Index)))
	{
		return nullptr;
	}
	return InputPins[Index];
}

UEdGraphPin* UVoxelGraphNodeBase::GetOutputPin(const int32 Index) const
{
	const TArray<UEdGraphPin*> OutputPins = GetOutputPins();
	if (!ensure(OutputPins.IsValidIndex(Index)))
	{
		return nullptr;
	}
	return OutputPins[Index];
}

UEdGraphPin* UVoxelGraphNodeBase::FindPinByPredicate_Unique(TFunctionRef<bool(UEdGraphPin* Pin)> Function) const
{
	UEdGraphPin* FoundPin = nullptr;
	for (UEdGraphPin* Pin : Pins)
	{
		if (!Function(Pin))
		{
			continue;
		}

		ensure(!FoundPin);
		FoundPin = Pin;
	}

	return FoundPin;
}

const UVoxelGraphSchemaBase* UVoxelGraphNodeBase::GetSchema() const
{
	return CastChecked<const UVoxelGraphSchemaBase>(Super::GetSchema());
}

TSharedPtr<FVoxelGraphToolkit> UVoxelGraphNodeBase::GetToolkit() const
{
	return CastChecked<UVoxelEdGraph>(GetGraph())->GetGraphToolkit();
}

void UVoxelGraphNodeBase::RefreshNode()
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit();
	if (!Toolkit)
	{
		return;
	}

	if (const TSharedPtr<SGraphEditor> GraphEditor = Toolkit->FindGraphEditor(GetGraph()))
	{
		GraphEditor->RefreshNode(*this);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelGraphNodeBase::CanRecombinePin(const UEdGraphPin& Pin) const
{
	if (!Pin.ParentPin)
	{
		return false;
	}

	ensure(Pin.ParentPin->bHidden);
	ensure(Pin.ParentPin->LinkedTo.Num() == 0);
	ensure(Pin.ParentPin->SubPins.Contains(&Pin));

	return true;
}

void UVoxelGraphNodeBase::RecombinePin(UEdGraphPin& Pin)
{
	Modify();

	UEdGraphPin* ParentPin = Pin.ParentPin;
	check(ParentPin);

	ensure(ParentPin->bHidden);
	ParentPin->bHidden = false;

	const TArray<UEdGraphPin*> SubPins = ParentPin->SubPins;
	ensure(SubPins.Num() > 0);

	for (UEdGraphPin* SubPin : SubPins)
	{
		SubPin->MarkAsGarbage();
		ensure(Pins.Remove(SubPin) == 1);
	}

	ensure(ParentPin->SubPins.Num() == 0);

	RefreshNode();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphNodeBase::FocusOnNode(UEdGraphNode* Node)
{
	FocusOnNodes({ Node });
}

void UVoxelGraphNodeBase::FocusOnNodes(const TArray<UEdGraphNode*>& Nodes)
{
	if (!ensure(Nodes.Num() > 0))
	{
		return;
	}

	const UEdGraphNode* FirstNode = Nodes[0];
	if (!ensure(FirstNode) ||
		!ensure(FirstNode->IsA<UVoxelGraphNodeBase>()) ||
		!ensure(FirstNode->GetGraph()))
	{
		return;
	}

	UObject* Asset = FirstNode->GetGraph()->GetOuter();
	if (!ensure(Asset))
	{
		return;
	}

	for (const UEdGraphNode* Node : Nodes)
	{
		if (!ensure(Node) ||
			!ensure(Node->GetGraph()) ||
			!ensure(Node->GetGraph()->GetOuter() == Asset))
		{
			return;
		}
	}

	const FVoxelGraphToolkit* Toolkit = FVoxelToolkit::OpenToolkit<FVoxelGraphToolkit>(Asset);
	if (!ensure(Toolkit))
	{
		return;
	}

	const TSharedPtr<SGraphEditor> GraphEditor = Toolkit->OpenGraphAndBringToFront(FirstNode->GetGraph(), true);
	if (!ensure(GraphEditor))
	{
		return;
	}

	GraphEditor->ClearSelectionSet();
	for (UEdGraphNode* Node : Nodes)
	{
		GraphEditor->SetNodeSelection(Node, true);
	}
	GraphEditor->ZoomToFit(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphNodeBase::ReconstructNode(bool bCreateOrphans)
{
	VOXEL_FUNCTION_COUNTER();

	TUniquePtr<TGuardValue<bool>> Guard;
	if (const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit())
	{
		Guard = MakeUnique<TGuardValue<bool>>(Toolkit->bDisableOnGraphChanged, true);
	}
	(void)Guard;

	Modify();
	PreReconstructNode();

	// Sanitize links
	for (UEdGraphPin* Pin : Pins)
	{
		Pin->PinType = FVoxelPinType(Pin->PinType).GetEdGraphPinType();
		Pin->LinkedTo.Remove(nullptr);

		for (UEdGraphPin* OtherPin : Pin->LinkedTo)
		{
			if (!OtherPin->GetOwningNode()->Pins.Contains(OtherPin))
			{
				Pin->LinkedTo.Remove(OtherPin);
			}
		}
	}

	// Move the existing pins to a saved array
	const TArray<UEdGraphPin*> OldPins = Pins;
	Pins.Reset();

	// Recreate the new pins
	AllocateDefaultPins();

	// Split new pins
	for (const UEdGraphPin* OldPin : OldPins)
	{
		if (OldPin->SubPins.Num() == 0)
		{
			continue;
		}

		UEdGraphPin* NewPin = FindPinByPredicate_Unique([&](const UEdGraphPin* Pin)
		{
			return
				Pin->PinName == OldPin->PinName &&
				Pin->PinType == OldPin->PinType &&
				Pin->Direction == OldPin->Direction;
		});
		if (!NewPin)
		{
			continue;
		}

		SplitPin(*NewPin);
	}

	TMap<UEdGraphPin*, UEdGraphPin*> NewPinsToOldPins;
	TMap<UEdGraphPin*, UEdGraphPin*> OldPinsToNewPins;

	// Map by name
	for (UEdGraphPin* OldPin : OldPins)
	{
		UEdGraphPin* NewPin = FindPinByPredicate_Unique([&](const UEdGraphPin* Pin)
		{
			if (Pin->Direction != OldPin->Direction ||
				Pin->PinName != OldPin->PinName)
			{
				return false;
			}

			if ((Pin->ParentPin != nullptr) != (OldPin->ParentPin != nullptr))
			{
				return false;
			}

			if (Pin->ParentPin &&
				OldPin->ParentPin &&
				Pin->ParentPin->PinName != OldPin->ParentPin->PinName)
			{
				return false;
			}

			return true;
		});

		if (!NewPin)
		{
			NewPin = FindPinByPredicate_Unique([&](const UEdGraphPin* Pin)
			{
				if (Pin->Direction != OldPin->Direction ||
					Pin->PinName != GetMigratedPinName(OldPin->PinName))
				{
					return false;
				}

				if ((Pin->ParentPin != nullptr) != (OldPin->ParentPin != nullptr))
				{
					return false;
				}

				if (Pin->ParentPin &&
					OldPin->ParentPin &&
					Pin->ParentPin->PinName != GetMigratedPinName(OldPin->ParentPin->PinName))
				{
					return false;
				}

				return true;
			});
		}

		if (!NewPin)
		{
			continue;
		}

		ensure(!OldPinsToNewPins.Contains(OldPin));
		ensure(!NewPinsToOldPins.Contains(NewPin));

		OldPinsToNewPins.Add(OldPin, NewPin);
		NewPinsToOldPins.Add(NewPin, OldPin);
	}

	if (!bCreateOrphans)
	{
		// Map by index if we're not creating orphans
		for (int32 Index = 0; Index < OldPins.Num(); Index++)
		{
			UEdGraphPin* OldPin = OldPins[Index];
			if (OldPinsToNewPins.Contains(OldPin) ||
				!Pins.IsValidIndex(Index))
			{
				continue;
			}

			UEdGraphPin* NewPin = Pins[Index];
			if (NewPinsToOldPins.Contains(NewPin))
			{
				continue;
			}

			ensure(!OldPinsToNewPins.Contains(OldPin));
			ensure(!NewPinsToOldPins.Contains(NewPin));

			OldPinsToNewPins.Add(OldPin, NewPin);
			NewPinsToOldPins.Add(NewPin, OldPin);
		}
	}

	TSet<UEdGraphPin*> MigratedOldPins;
	for (const auto& It : OldPinsToNewPins)
	{
		UEdGraphPin* OldPin = It.Key;
		UEdGraphPin* NewPin = It.Value;

		ensure(!OldPin->bOrphanedPin);

		if (TryMigratePin(OldPin, NewPin))
		{
			ensure(OldPin->LinkedTo.Num() == 0);
			MigratedOldPins.Add(OldPin);
			continue;
		}

		if (bCreateOrphans)
		{
			continue;
		}

		// If we're not going to create an orphan try to keep the default value
		TryMigrateDefaultValue(OldPin, NewPin);
	}

	struct FConnectionToRestore
	{
		UEdGraphPin* Pin = nullptr;
		UEdGraphPin* LinkedTo = nullptr;
		UEdGraphPin* OrphanedPin = nullptr;
	};
	TArray<FConnectionToRestore> ConnectionsToRestore;

	// Throw away the original pins
	for (UEdGraphPin* OldPin : OldPins)
	{
		UEdGraphPin* OrphanedPin = nullptr;

		if ((bCreateOrphans || OldPin->bOrphanedPin) &&
			!MigratedOldPins.Contains(OldPin) &&
			(OldPin->LinkedTo.Num() > 0 || !OldPin->DoesDefaultValueMatchAutogenerated()))
		{
			FString NewName = OldPin->PinName.ToString();
			if (!NewName.StartsWith("ORPHANED_"))
			{
				// Avoid collisions
				NewName = "ORPHANED_" + NewName;
			}

			OrphanedPin = CreatePin(OldPin->Direction, OldPin->PinType, *NewName);
			OrphanedPin->bOrphanedPin = true;
			ensure(TryMigratePin(OldPin, OrphanedPin));
			OrphanedPin->PinFriendlyName = OldPin->PinFriendlyName.IsEmpty() ? FText::FromName(OldPin->PinName) : OldPin->PinFriendlyName;
		}

		if (UEdGraphPin* NewPin = OldPinsToNewPins.FindRef(OldPin))
		{
			for (UEdGraphPin* LinkedTo : OldPin->LinkedTo)
			{
				FConnectionToRestore ConnectionToRestore;
				ConnectionToRestore.Pin = NewPin;
				ConnectionToRestore.LinkedTo = LinkedTo;
				ConnectionToRestore.OrphanedPin = OrphanedPin;
				ConnectionsToRestore.Add(ConnectionToRestore);
			}
		}

		OldPin->Modify();
		DestroyPin(OldPin);
	}

	// Check bNotConnectable
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->bNotConnectable)
		{
			Pin->BreakAllPinLinks();
		}
	}

	PostReconstructNode();

	for (const FConnectionToRestore& Connection : ConnectionsToRestore)
	{
		// Never promote nor break connections
		if (GetSchema()->CanCreateConnection(Connection.Pin, Connection.LinkedTo).Response != CONNECT_RESPONSE_MAKE)
		{
			continue;
		}

		if (!ensure(GetSchema()->TryCreateConnection(Connection.Pin, Connection.LinkedTo)))
		{
			continue;
		}

		if (Connection.OrphanedPin)
		{
			Connection.OrphanedPin->BreakLinkTo(Connection.LinkedTo);
		}
	}

	RefreshNode();
	FixupPreviewSettings();
}

void UVoxelGraphNodeBase::FixupPreviewSettings()
{
	VOXEL_FUNCTION_COUNTER();

	TMap<FName, FVoxelPinPreviewSettings> NameToPreviewSettings;
	for (const FVoxelPinPreviewSettings& Settings : PreviewSettings)
	{
		ensure(!NameToPreviewSettings.Contains(Settings.PinName));
		NameToPreviewSettings.Add(Settings.PinName, Settings);
	}

	PreviewSettings.Reset();

	for (const UEdGraphPin* Pin : GetOutputPins())
	{
		if (Pin->ParentPin ||
			Pin->bOrphanedPin)
		{
			continue;
		}

		FVoxelPinPreviewSettings Settings = NameToPreviewSettings.FindRef(Pin->PinName);
		Settings.PinName = Pin->PinName;
		Settings.PinType = Pin->PinType;

		if (!Settings.PreviewHandler.IsA<FVoxelPreviewHandler>() ||
			!Settings.PreviewHandler.Get<FVoxelPreviewHandler>().SupportsType(Settings.PinType))
		{
			Settings.PreviewHandler.Reset();

			for (const FVoxelPreviewHandler* Handler : FVoxelPreviewHandler::GetHandlers())
			{
				if (Handler->SupportsType(Settings.PinType))
				{
					Settings.PreviewHandler = FVoxelInstancedStruct(Handler->GetStruct());
					break;
				}
			}
		}

		PreviewSettings.Add(Settings);
	}

	if (!FindPin(PreviewedPin))
	{
		for (const FVoxelPinPreviewSettings& Settings : PreviewSettings)
		{
			PreviewedPin = Settings.PinName;
			break;
		}
	}

	if (const TSharedPtr<FVoxelGraphToolkit>& Toolkit = GetToolkit())
	{
		Toolkit->RefreshPreviewSettings();
	}
}

void UVoxelGraphNodeBase::AllocateDefaultPins()
{
	TSet<FName> Names;
	for (const UEdGraphPin* Pin : Pins)
	{
		ensure(!Names.Contains(Pin->PinName));
		Names.Add(Pin->PinName);
	}

	InputPinCategories = {};
	OutputPinCategories = {};

	TSet<FName> ValidCategories;
	for (const UEdGraphPin* Pin : Pins)
	{
		FName PinCategory = GetPinCategory(*Pin);
		ValidCategories.Add(PinCategory);

		switch (Pin->Direction)
		{
		default: check(false);
		case EGPD_Input: InputPinCategories.Add(PinCategory); break;
		case EGPD_Output: OutputPinCategories.Add(PinCategory); break;
		}
	}

	CollapsedInputCategories = CollapsedInputCategories.Intersect(ValidCategories);
	CollapsedOutputCategories = CollapsedOutputCategories.Intersect(ValidCategories);

	FixupPreviewSettings();
}

void UVoxelGraphNodeBase::ReconstructNode()
{
	ReconstructNode(true);
}

FLinearColor UVoxelGraphNodeBase::GetNodeTitleColor() const
{
	return FLinearColor::Black;
}

void UVoxelGraphNodeBase::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (!FromPin)
	{
		return;
	}

	const UEdGraphSchema* Schema = GetSchema();

	// Check non-promotable pins first
	for (UEdGraphPin* Pin : Pins)
	{
		const FPinConnectionResponse Response = Schema->CanCreateConnection(FromPin, Pin);

		if (Response.Response == CONNECT_RESPONSE_MAKE)
		{
			Schema->TryCreateConnection(FromPin, Pin);
			return;
		}
	}

	for (UEdGraphPin* Pin : Pins)
	{
		const FPinConnectionResponse Response = Schema->CanCreateConnection(FromPin, Pin);

		if (Response.Response == CONNECT_RESPONSE_MAKE_WITH_PROMOTION)
		{
			Schema->TryCreateConnection(FromPin, Pin);
			return;
		}
		else if (Response.Response == CONNECT_RESPONSE_BREAK_OTHERS_A)
		{
			// The pin we are creating from already has a connection that needs to be broken. We want to "insert" the new node in between, so that the output of the new node is hooked up too
			UEdGraphPin* OldLinkedPin = FromPin->LinkedTo[0];
			check(OldLinkedPin);

			FromPin->BreakAllPinLinks();

			// Hook up the old linked pin to the first valid output pin on the new node
			for (UEdGraphPin* InnerPin : Pins)
			{
				if (Schema->CanCreateConnection(OldLinkedPin, InnerPin).Response != CONNECT_RESPONSE_MAKE)
				{
					continue;
				}

				Schema->TryCreateConnection(OldLinkedPin, InnerPin);
				break;
			}

			Schema->TryCreateConnection(FromPin, Pin);
			return;
		}
	}
}

void UVoxelGraphNodeBase::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	// If the default value is manually set then treat it as if the value was reset to default and remove the orphaned pin
	if (Pin->bOrphanedPin &&
		Pin->DoesDefaultValueMatchAutogenerated())
	{
		PinConnectionListChanged(Pin);
	}

	UVoxelGraphSchemaBase::OnGraphChanged(this);
}

void UVoxelGraphNodeBase::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (!Pin->bOrphanedPin ||
		Pin->LinkedTo.Num() > 0 ||
		!Pin->DoesDefaultValueMatchAutogenerated())
	{
		return;
	}

	// If we're not linked and this pin should no longer exist as part of the node, remove it

	RemovePin(Pin);
	RefreshNode();
}

void UVoxelGraphNodeBase::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	if (!bDuplicateForPIE)
	{
		CreateNewGuid();
	}
}

void UVoxelGraphNodeBase::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
}

void UVoxelGraphNodeBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive ||
		PropertyChangedEvent.ChangeType == EPropertyChangeType::Redirected)
	{
		return;
	}

	RefreshNode();
	UVoxelGraphSchemaBase::OnGraphChanged(this);
}