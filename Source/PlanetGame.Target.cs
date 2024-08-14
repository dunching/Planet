// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlanetGameTarget : TargetRules
{
	public PlanetGameTarget( TargetInfo Target) : base(Target)
    {
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        DefaultBuildSettings = BuildSettingsVersion.Latest;

        Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] {
            "Planet",
            "Tools",
        } );
	}
}
