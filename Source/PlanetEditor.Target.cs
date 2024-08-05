// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlanetEditorTarget : TargetRules
{
	public PlanetEditorTarget( TargetInfo Target) : base(Target)
    {
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        // bStrictConformanceMode = true;

        // 屏蔽之后不能提调试,具体原因未知
        BuildEnvironment = TargetBuildEnvironment.Unique;
        // bOverrideBuildEnvironment = false;

        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;

        ExtraModuleNames.AddRange( new string[] {
            "Planet",
            "PlanetEditor",
            "Tools",
        } );
	}
}
