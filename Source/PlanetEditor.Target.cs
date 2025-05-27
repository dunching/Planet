// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlanetEditorTarget : TargetRules
{
	public PlanetEditorTarget( TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        DefaultBuildSettings = BuildSettingsVersion.Latest;

        // bStrictConformanceMode = true;

        // 屏蔽之后不能提调试,具体原因未知
        // BuildEnvironment = TargetBuildEnvironment.Unique;
        BuildEnvironment = TargetBuildEnvironment.Shared;
        // bOverrideBuildEnvironment = false;

        ExtraModuleNames.AddRange( new string[] {
            "Planet",
            "PlanetEditor",
            "Tools",
            "GameplayTagsLibrary",
            "CommonType",
            "AssetRef",
            "GameOptions",
        } );
	}
}
