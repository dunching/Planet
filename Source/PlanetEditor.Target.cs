// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlanetEditorTarget : TargetRules
{
	public PlanetEditorTarget( TargetInfo Target) : base(Target)
    {
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        DefaultBuildSettings = BuildSettingsVersion.Latest;

        // bStrictConformanceMode = true;

        // ����֮���������,����ԭ��δ֪
        // BuildEnvironment = TargetBuildEnvironment.Unique;
        BuildEnvironment = TargetBuildEnvironment.Shared;
        // bOverrideBuildEnvironment = false;

        Type = TargetType.Editor;

        ExtraModuleNames.AddRange( new string[] {
            "Planet",
            "PlanetEditor",
            "Tools",
        } );
	}
}
