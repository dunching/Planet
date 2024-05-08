// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelBlueprintFunctionLibrary.h"
#include "VoxelActor.h"
#include "AssetToolsModule.h"

EVoxelSuccess UVoxelBlueprintFunctionLibrary::CreateNewVoxelGraphInstanceFromVoxelActor(
	AVoxelActor* SourceActor,
	const FString AssetPathAndName,
	UVoxelGraphInstance*& VoxelGraphInstanceAsset)
{
	if (!SourceActor)
	{
		VOXEL_MESSAGE(Error, "Actor is Null");
		return EVoxelSuccess::Failed;
	}

	if (CreateNewVoxelGraphInstanceFromGraph(SourceActor->GetGraph(), AssetPathAndName, VoxelGraphInstanceAsset) != EVoxelSuccess::Succeeded)
	{
		return EVoxelSuccess::Failed;
	}

	ApplyParametersToGraphInstance(SourceActor->ParameterContainer, VoxelGraphInstanceAsset);

	return EVoxelSuccess::Succeeded;
}

EVoxelSuccess UVoxelBlueprintFunctionLibrary::CreateNewVoxelGraphInstanceFromGraph(
	UVoxelGraphInterface* SourceGraph,
	FString AssetPathAndName,
	UVoxelGraphInstance*& VoxelGraphInstanceAsset)
{
	if (!SourceGraph)
	{
		VOXEL_MESSAGE(Error, "From Graph is Null");
		return EVoxelSuccess::Failed;
	}

	if (!SourceGraph->IsAsset())
	{
		VOXEL_MESSAGE(Error, "From Graph must be asset");
		return EVoxelSuccess::Failed;
	}

	AssetPathAndName = UPackageTools::SanitizePackageName(AssetPathAndName);

	IAssetTools& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

	FString NewAssetName;
	FString PackageName;
	AssetToolsModule.CreateUniqueAssetName(AssetPathAndName, "", PackageName, NewAssetName);

	UVoxelGraphInstance* NewInstanceAsset = Cast<UVoxelGraphInstance>(AssetToolsModule.CreateAsset(NewAssetName, FPackageName::GetLongPackagePath(PackageName), UVoxelGraphInstance::StaticClass(), nullptr));
	if (!ensure(NewInstanceAsset))
	{
		VOXEL_MESSAGE(Error, "Failed to create new asset");
		return EVoxelSuccess::Failed;
	}

	{
		FVoxelTransaction Transaction(NewInstanceAsset);
		NewInstanceAsset->SetParentGraph(SourceGraph);
	}

	VoxelGraphInstanceAsset = NewInstanceAsset;

	return EVoxelSuccess::Succeeded;
}

EVoxelSuccess UVoxelBlueprintFunctionLibrary::ApplyParametersToGraphInstance(
	UVoxelParameterContainer* SourceParametersContainer,
	UVoxelGraphInstance* DestinationGraph)
{
	if (!DestinationGraph ||
		!DestinationGraph->ParameterContainer ||
		!DestinationGraph->ParameterContainer->GetParameterView())
	{
		VOXEL_MESSAGE(Error, "Graph is Null");
		return EVoxelSuccess::Failed;
	}

	if (!SourceParametersContainer ||
		!SourceParametersContainer->GetParameterView())
	{
		VOXEL_MESSAGE(Error, "Parameters Container is Null");
		return EVoxelSuccess::Failed;
	}

	FVoxelTransaction Transaction(DestinationGraph);
	DestinationGraph->ParameterContainer->ValueOverrides = SourceParametersContainer->ValueOverrides;
	DestinationGraph->ParameterContainer->Fixup();

	return EVoxelSuccess::Succeeded;
}