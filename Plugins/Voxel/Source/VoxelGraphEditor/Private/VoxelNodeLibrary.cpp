// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelNodeLibrary.h"
#include "VoxelFunctionNode.h"
#include "VoxelFunctionLibrary.h"

FVoxelNodeLibrary* GVoxelNodeLibrary = nullptr;

VOXEL_RUN_ON_STARTUP_EDITOR_COOK(RegisterVoxelNodeLibrary)
{
	GVoxelNodeLibrary = new FVoxelNodeLibrary();
	GOnVoxelModuleUnloaded_DoCleanup.AddLambda([]
	{
		delete GVoxelNodeLibrary;
		GVoxelNodeLibrary = nullptr;
	});

	FCoreUObjectDelegates::ReloadCompleteDelegate.AddLambda([](EReloadCompleteReason)
	{
		delete GVoxelNodeLibrary;
		GVoxelNodeLibrary = new FVoxelNodeLibrary();
	});
}

FVoxelNodeLibrary::FVoxelNodeLibrary()
{
	VOXEL_FUNCTION_COUNTER();

	for (UScriptStruct* Struct : GetDerivedStructs<FVoxelNode>())
	{
		if (Struct->HasMetaData(STATIC_FNAME("Abstract")) ||
			Struct->HasMetaData(STATIC_FNAME("Internal")))
		{
			continue;
		}

		Nodes.Add(TVoxelInstancedStruct<FVoxelNode>(Struct).Release());
	}
	for (const TSubclassOf<UVoxelFunctionLibrary>& Class : GetDerivedClasses<UVoxelFunctionLibrary>())
	{
		for (UFunction* Function : GetClassFunctions(Class))
		{
			if (Function->HasMetaData(STATIC_FNAME("Internal")))
			{
				continue;
			}

			FVoxelFunctionNode* Node = new (GVoxelMemory) FVoxelFunctionNode();
			Node->SetFunction_EditorOnly(Function);
			Nodes.Add(Node);
		}
	}
	Nodes.Shrink();

#if VOXEL_DEBUG
	for (const FVoxelNode* Node : Nodes)
	{
		(void)Node->GetCategory();
		(void)Node->GetDisplayName();
		(void)Node->GetTooltip();
	}
#endif

	for (const FVoxelNode* Node : Nodes)
	{
		if (!Node->GetMetadataContainer().HasMetaDataHierarchical(STATIC_FNAME("NativeMakeFunc")))
		{
			continue;
		}

		const FVoxelPin& OutputPin = Node->GetUniqueOutputPin();

		FVoxelPinTypeSet Types;
		if (OutputPin.IsPromotable())
		{
			Types = Node->GetPromotionTypes(OutputPin);
		}
		else
		{
			Types.Add(OutputPin.GetType());
		}

		for (const FVoxelPinType& Type : Types.GetTypes())
		{
			ensure(!MakeNodes.Contains(Type));
			MakeNodes.Add(Type, Node);
		}
	}
	MakeNodes.Compact();

	for (const FVoxelNode* Node : Nodes)
	{
		if (!Node->GetMetadataContainer().HasMetaDataHierarchical(STATIC_FNAME("NativeBreakFunc")))
		{
			continue;
		}

		const FVoxelPin& InputPin = Node->GetUniqueInputPin();

		FVoxelPinTypeSet Types;
		if (InputPin.IsPromotable())
		{
			Types = Node->GetPromotionTypes(InputPin);
		}
		else
		{
			Types.Add(InputPin.GetType());
		}

		for (const FVoxelPinType& Type : Types.GetTypes())
		{
			ensure(!BreakNodes.Contains(Type));
			BreakNodes.Add(Type, Node);
		}
	}
	BreakNodes.Compact();

	for (const FVoxelNode* Node : Nodes)
	{
		if (!Node->GetMetadataContainer().HasMetaDataHierarchical(STATIC_FNAME("Autocast")))
		{
			continue;
		}

		const TSharedRef<FVoxelNode> NodeCopy = Node->MakeSharedCopy();

		FVoxelPin* FirstInputPin = nullptr;
		FVoxelPin* FirstOutputPin = nullptr;
		for (FVoxelPin& Pin : NodeCopy->GetPins())
		{
			if (Pin.bIsInput && !FirstInputPin)
			{
				FirstInputPin = &Pin;
			}
			if (!Pin.bIsInput && !FirstOutputPin)
			{
				FirstOutputPin = &Pin;
			}
		}
		check(FirstInputPin);
		check(FirstOutputPin);

		FVoxelPinTypeSet InputTypes;
		if (FirstInputPin->IsPromotable())
		{
			InputTypes = NodeCopy->GetPromotionTypes(*FirstInputPin);
		}
		else
		{
			InputTypes.Add(FirstInputPin->GetType());
		}

		FVoxelPinTypeSet OutputTypes;
		if (FirstOutputPin->IsPromotable())
		{
			OutputTypes = NodeCopy->GetPromotionTypes(*FirstOutputPin);
		}
		else
		{
			OutputTypes.Add(FirstOutputPin->GetType());
		}

		TSet<TPair<FVoxelPinType, FVoxelPinType>> Pairs;
		for (const FVoxelPinType& InputType : InputTypes.GetTypes())
		{
			if (FirstInputPin->IsPromotable())
			{
				NodeCopy->PromotePin(*FirstInputPin, InputType);
			}

			for (const FVoxelPinType& OutputType : OutputTypes.GetTypes())
			{
				if (FirstOutputPin->IsPromotable())
				{
					NodeCopy->PromotePin(*FirstOutputPin, OutputType);
				}

				Pairs.Add({ FirstInputPin->GetType(), FirstOutputPin->GetType() });
			}
		}

		for (const TPair<FVoxelPinType, FVoxelPinType>& Pair : Pairs)
		{
			ensure(!CastNodes.Contains(Pair));
			CastNodes.Add(Pair, Node);
		}
	}
	CastNodes.Compact();

	for (const FVoxelNode* Node : Nodes)
	{
		StructToNodes.Add(Node->GetStruct(), Node);
	}
}

FVoxelNodeLibrary::~FVoxelNodeLibrary()
{
	for (const FVoxelNode* Node : Nodes)
	{
		FVoxelMemory::Delete(Node);
	}
}

const FVoxelNodeLibrary& FVoxelNodeLibrary::Get()
{
	return *GVoxelNodeLibrary;
}