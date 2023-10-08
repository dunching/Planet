// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphInterface.h"
#include "VoxelGraph.h"
#if WITH_EDITOR
#include "ObjectTools.h"
#endif

FString UVoxelGraphInterface::GetGraphName() const
{
	FString Name = GetName();
	Name.RemoveFromStart("VG_");
	Name.RemoveFromStart("VGI_");
	return FName::NameToDisplayString(Name, false);
}

void UVoxelGraphInterface::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	const UVoxelGraph* Graph = GetGraph();
	if (!Graph ||
		!Graph->bEnableThumbnail)
	{
		ThumbnailTools::CacheEmptyThumbnail(GetFullName(), GetOutermost());
	}
#endif
}