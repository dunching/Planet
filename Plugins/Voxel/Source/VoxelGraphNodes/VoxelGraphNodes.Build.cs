// Copyright Voxel Plugin, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelGraphNodes : ModuleRules_Voxel
{
    public VoxelGraphNodes(ReadOnlyTargetRules Target) : base(Target)
    {
	    PublicDependencyModuleNames.AddRange(
		    new string[]
		    {
			    "PhysicsCore",
			    "Chaos",
		    }
	    );
    }
}