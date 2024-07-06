// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlanetGameTarget : TargetRules
{
	public PlanetGameTarget( TargetInfo Target) : base(Target)
    {
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] {
            "Planet",
            "Tools",
        } );
	}
}
