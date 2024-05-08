// Copyright Voxel Plugin, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelContentEditor : ModuleRules_Voxel
{
    public VoxelContentEditor(ReadOnlyTargetRules Target) : base(Target)
    {
	    PublicDependencyModuleNames.AddRange(
		    new string[]
		    {
			    "ToolMenus",
			    "ToolWidgets",
			    "WidgetCarousel",
			    "ApplicationCore",
			    "Json",
			    "HTTP",
		    });
    }
}