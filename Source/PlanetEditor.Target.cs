// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlanetEditorTarget : TargetRules
{
	public PlanetEditorTarget( TargetInfo Target) : base(Target)
    {
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        //  bStrictConformanceMode = true;

        BuildEnvironment = TargetBuildEnvironment.Unique;

        Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] {
            "Planet",
            "PlanetEditor",
            "Tools",
        } );
	}
}
