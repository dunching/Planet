// Copyright Voxel Plugin, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelLandmass : ModuleRules_Voxel
{
    public VoxelLandmass(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "VoxelGraphNodes",
                "Chaos",
                "Landscape",
                "MeshDescription",
            }
        );
    }
}