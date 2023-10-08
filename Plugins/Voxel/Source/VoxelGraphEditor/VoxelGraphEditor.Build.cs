// Copyright Voxel Plugin, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelGraphEditor : ModuleRules_Voxel
{
    public VoxelGraphEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "VoxelGraphNodes",
                "VoxelContentEditor",
                "MainFrame",
                "ToolMenus",
                "MessageLog",
                "CurveEditor",
                "GraphEditor",
                "KismetWidgets",
                "EditorWidgets",
                "BlueprintGraph",
                "ApplicationCore",
                "SharedSettingsWidgets",
                "InteractiveToolsFramework",
                "EditorInteractiveToolsFramework",

                // For SItemSelector
                "NiagaraEditor",
            }
        );
    }
}