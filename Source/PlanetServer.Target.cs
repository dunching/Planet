// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlanetServerTarget : TargetRules
{
	public PlanetServerTarget( TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;

        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        DefaultBuildSettings = BuildSettingsVersion.Latest;

		ExtraModuleNames.AddRange( new string[] {
            "Planet",
            "Tools",
            "GameplayTagsLibrary",
            "CommonType",
            "AssetRef",
            "GameOptions",
        } );
	}
}
