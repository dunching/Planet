// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "K2Node_QueryVoxelChannel.h"
#include "VoxelChannel.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "FunctionLibrary/VoxelGraphFunctionLibrary.h"

void UK2Node_QueryVoxelChannelBase::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	UpdateChannel();
}

void UK2Node_QueryVoxelChannelBase::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	if (!Context->Pin ||
		!IsPinWildcard(*Context->Pin) ||
		HasChannel())
	{
		return;
	}

	AddConvertPinContextAction(Menu, Context, FVoxelPinTypeSet::AllUniforms());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UK2Node_QueryVoxelChannelBase::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	const FEdGraphPinType OldPinType = Pin->PinType;

	Super::NotifyPinConnectionListChanged(Pin);

	if (!IsPinWildcard(*Pin))
	{
		return;
	}

	if (HasChannel() &&
		Pin->PinType != GetValuePinType() &&
		OldPinType == GetValuePinType())
	{
		Pin->PinType = GetValuePinType();
		return;
	}

	if (HasChannel() ||
		Pin->PinType.PinCategory != STATIC_FNAME("wildcard"))
	{
		return;
	}

	if (Pin->PinType.PinCategory == STATIC_FNAME("wildcard") &&
		OldPinType.PinCategory != STATIC_FNAME("wildcard"))
	{
		Pin->PinType = OldPinType;
		return;
	}

	SetType(FVoxelPinType::MakeFromK2(Pin->PinType));
	UpdateChannel();
}

void UK2Node_QueryVoxelChannelBase::PostReconstructNode()
{
	Super::PostReconstructNode();

	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
	for (UEdGraphPin* Pin : GetAllPins())
	{
		if (!IsPinWildcard(*Pin))
		{
			continue;
		}

		Pin->PinType = GetValuePinType();
		if (Pin->DoesDefaultValueMatchAutogenerated())
		{
			Schema->ResetPinToAutogeneratedDefaultValue(Pin, false);
		}
	}
}

bool UK2Node_QueryVoxelChannelBase::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (!IsPinWildcard(*MyPin) ||
		HasChannel())
	{
		return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
	}

	if (!CanAutoConvert(*MyPin, *OtherPin, OutReason))
	{
		return true;
	}

	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
}

void UK2Node_QueryVoxelChannelBase::PostLoad()
{
	Super::PostLoad();

	UpdateChannel();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UK2Node_QueryVoxelChannelBase::IsPinWildcard(const UEdGraphPin& Pin) const
{
	return Pin.PinName == STATIC_FNAME("Value");
}

void UK2Node_QueryVoxelChannelBase::OnPinTypeChange(UEdGraphPin& Pin, const FVoxelPinType& NewType)
{
	SetType(NewType);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UK2Node_QueryVoxelChannelBase::SetType(const FVoxelPinType& NewType)
{
	if (ValueType == NewType.GetExposedType())
	{
		return;
	}

	ValueType = NewType.GetExposedType();

	UEdGraphPin* ValuePin = FindPin(STATIC_FNAME("Value"));
	if (!ensure(ValuePin))
	{
		return;
	}

	if (ValuePin->LinkedTo.Num() > 0 &&
		ValuePin->PinType != GetValuePinType())
	{
		if (ValuePin->PinFriendlyName.IsEmpty())
		{
			ValuePin->PinFriendlyName = FText::FromName(ValuePin->PinName);
		}

		ValuePin->PinName = CreateUniquePinName(ValuePin->PinName);
	}

	ReconstructNode();

	// Let the graph know to refresh
	GetGraph()->NotifyGraphChanged();

	UBlueprint* Blueprint = GetBlueprint();
	if (!Blueprint->bBeingCompiled)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		Blueprint->BroadcastChanged();
	}
}

bool UK2Node_QueryVoxelChannelBase::HasChannel() const
{
	const UEdGraphPin* Pin = FindPin(STATIC_FNAME("Channel"));
	if (!ensure(Pin) ||
		Pin->LinkedTo.Num() > 0)
	{
		return false;
	}

	const FName ChannelName(Pin->DefaultValue);
	return GVoxelChannelManager->FindChannelDefinition(ChannelName).IsSet();
}

void UK2Node_QueryVoxelChannelBase::UpdateChannel()
{
	const UEdGraphPin* Pin = FindPin(STATIC_FNAME("Channel"));
	if (!ensure(Pin))
	{
		return;
	}

	const FName ChannelName(Pin->DefaultValue);
	const TOptional<FVoxelChannelDefinition> ChannelDefinition = GVoxelChannelManager->FindChannelDefinition(ChannelName);

	if (!ChannelDefinition)
	{
		DelegateOwner = {};
		return;
	}

	SetType(ChannelDefinition->Type);

	DelegateOwner = MakeVoxelShared<int32>();
	GVoxelChannelManager->OnChannelDefinitionsChanged_GameThread.Add(MakeWeakPtrDelegate(DelegateOwner, [this]
	{
		UpdateChannel();
	}));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UK2Node_QueryVoxelChannel::UK2Node_QueryVoxelChannel()
{
	FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UVoxelGraphFunctionLibrary, K2_QueryVoxelChannel), UVoxelGraphFunctionLibrary::StaticClass());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UK2Node_MultiQueryVoxelChannel::UK2Node_MultiQueryVoxelChannel()
{
	FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UVoxelGraphFunctionLibrary, K2_MultiQueryVoxelChannel), UVoxelGraphFunctionLibrary::StaticClass());
}

FEdGraphPinType UK2Node_MultiQueryVoxelChannel::GetValuePinType() const
{
	return ValueType.GetBufferType().GetEdGraphPinType_K2();
}