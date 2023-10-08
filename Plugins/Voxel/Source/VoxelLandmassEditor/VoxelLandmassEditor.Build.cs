// Copyright Voxel Plugin, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelLandmassEditor : ModuleRules_Voxel
{
    public VoxelLandmassEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "VoxelLandmass",
                "VoxelGraphNodes",
                "Landscape",
                "InteractiveToolsFramework",
#if UE_5_0_OR_LATER
#else
                "EditorInteractiveToolsFramework",
#endif
            });
    }
}