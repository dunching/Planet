// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphParameterNodeBase.h"
#include "VoxelGraphSchema.h"

FVoxelGraphParameter* UVoxelGraphParameterNodeBase::GetParameter() const
{
	UVoxelGraph* Graph = GetTypedOuter<UVoxelGraph>();
	if (!ensure(Graph))
	{
		return nullptr;
	}

	FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(Guid);
	if (!Parameter)
	{
		return nullptr;
	}

	if (!ensure(Parameter->ParameterType == GetParameterType()))
	{
		return nullptr;
	}

	return Parameter;
}

const FVoxelGraphParameter& UVoxelGraphParameterNodeBase::GetParameterSafe() const
{
	if (FVoxelGraphParameter* Parameter = GetParameter())
	{
		return *Parameter;
	}

	return CachedParameter;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphParameterNodeBase::AllocateDefaultPins()
{
	if (const FVoxelGraphParameter* Parameter = GetParameter())
	{
		AllocateParameterPins(*Parameter);
		CachedParameter = *Parameter;
	}
	else
	{
		AllocateParameterPins(CachedParameter);

		for (UEdGraphPin* Pin : Pins)
		{
			Pin->bOrphanedPin = true;
		}
	}

	Super::AllocateDefaultPins();
}

FLinearColor UVoxelGraphParameterNodeBase::GetNodeTitleColor() const
{
	return GetSchema()->GetPinTypeColor(GetParameterSafe().Type.GetEdGraphPinType());
}

void UVoxelGraphParameterNodeBase::PrepareForCopying()
{
	Super::PrepareForCopying();

	const FVoxelGraphParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return;
	}

	CachedParameter = *Parameter;
}

void UVoxelGraphParameterNodeBase::PostPasteNode()
{
	Super::PostPasteNode();

	UVoxelGraph* Graph = GetTypedOuter<UVoxelGraph>();
	if (!ensure(Graph))
	{
		return;
	}

	if (Graph->Parameters.FindByKey(Guid))
	{
		return;
	}

	const FVoxelGraphParameter* Parameter = Graph->Parameters.FindByKey(CachedParameter.Name);
	if (Parameter &&
		Parameter->Type == CachedParameter.Type &&
		Parameter->ParameterType == GetParameterType())
	{
		// Update Guid
		Guid = Parameter->Guid;
		return;
	}

	// Add new parameter
	// Regenerate guid to be safe
	Guid = FGuid::NewGuid();
	CachedParameter.Guid = Guid;

	Graph->Parameters.Add(CachedParameter);
}