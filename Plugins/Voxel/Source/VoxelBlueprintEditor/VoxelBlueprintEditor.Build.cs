// Copyright Voxel Plugin, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelBlueprintEditor : ModuleRules_Voxel
{
	public VoxelBlueprintEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"VoxelBlueprint",
				"VoxelGraphEditor",
				"GraphEditor",
				"BlueprintGraph",
			}
		);
	}
}