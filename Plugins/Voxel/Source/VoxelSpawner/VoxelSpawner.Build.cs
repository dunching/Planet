// Copyright Voxel Plugin, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelSpawner : ModuleRules_Voxel
{
    public VoxelSpawner(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "VoxelGraphNodes",
            }
        );

        SetupModulePhysicsSupport(Target);
    }
}