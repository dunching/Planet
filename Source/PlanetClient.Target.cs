// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlanetClientTarget : TargetRules
{
	public PlanetClientTarget( TargetInfo Target) : base(Target)
    {
		Type = TargetType.Client;
		
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        DefaultBuildSettings = BuildSettingsVersion.Latest;

		ExtraModuleNames.AddRange( new string[] {
	        
			"AssetRef",
			"ChallengeSystem",
			"CommonType",
			"GameOptions",
			"GameplayTagsLibrary",
			"Planet",
			"Planet_GroupManagger",
			"Planet_GuideSystem",
			"Planet_InputProcessor",
			"Planet_ItemProxy",
			"Planet_NPCGenerator",
			"Tools",
        } );
	}
}
