// Copyright Voxel Plugin, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelGraphCore : ModuleRules_Voxel
{
    public VoxelGraphCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"TraceLog",
				"Slate",
				"SlateCore",
			}
		);

		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"MessageLog",
				}
			);
		}
	}
}