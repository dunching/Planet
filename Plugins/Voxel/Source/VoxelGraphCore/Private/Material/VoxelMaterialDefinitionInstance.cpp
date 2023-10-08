// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Material/VoxelMaterialDefinitionInstance.h"
#include "Material/VoxelMaterialDefinition.h"
#include "VoxelParameterContainer.h"

DEFINE_VOXEL_FACTORY(UVoxelMaterialDefinitionInstance);

UVoxelMaterialDefinitionInstance::UVoxelMaterialDefinitionInstance()
{
	ParameterContainer = CreateDefaultSubobject<UVoxelParameterContainer>("ParameterContainer");
	ParameterContainer->OnChanged.AddWeakLambda(this, [this]
	{
		if (UVoxelMaterialDefinition* Definition = GetDefinition())
		{
			Definition->QueueRebuildTextures();
		}
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelMaterialDefinitionInstance::SetParentDefinition(UVoxelMaterialDefinitionInterface* NewParentDefinition)
{
	ParameterContainer->SetProvider(NewParentDefinition);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UVoxelMaterialDefinition* UVoxelMaterialDefinitionInstance::GetDefinition() const
{
	ParameterContainer->FixupProvider();

	const UVoxelMaterialDefinitionInterface* Interface = Cast<UVoxelMaterialDefinitionInterface>(ParameterContainer->Provider);
	if (!Interface)
	{
		ensure(!ParameterContainer->Provider);
		return nullptr;
	}
	return Interface->GetDefinition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelMaterialDefinitionInstance::PostLoad()
{
	Super::PostLoad();

	if (Parent_DEPRECATED)
	{
		ensure(!ParameterContainer->Provider);
		ParameterContainer->Provider = Parent_DEPRECATED.Get();
		Parent_DEPRECATED = {};

		ParameterCollection_DEPRECATED.MigrateTo(*ParameterContainer);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

IVoxelParameterProvider* UVoxelMaterialDefinitionInstance::GetSourceProvider()
{
	return ParameterContainer;
}