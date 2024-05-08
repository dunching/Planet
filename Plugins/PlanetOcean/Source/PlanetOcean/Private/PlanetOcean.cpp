// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlanetOcean.h"
#include "Interfaces/IPluginManager.h"


#define LOCTEXT_NAMESPACE "FPlanetOceanModule"

void FPlanetOceanModule::StartupModule()
{
	const IPlugin& Plugin = *IPluginManager::Get().FindPlugin("PlanetOcean");
	const FString ShaderDirectory = FPaths::ConvertRelativePathToFull(Plugin.GetBaseDir() / "Shaders/Private");
	AddShaderSourceDirectoryMapping("/PlanetOcean/Shaders", ShaderDirectory);
}

void FPlanetOceanModule::ShutdownModule()
{
	ResetAllShaderSourceDirectoryMappings();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPlanetOceanModule, PlanetOcean)