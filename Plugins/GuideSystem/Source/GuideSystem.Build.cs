// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class GuideSystem : ModuleRules
{
	public GuideSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		DefaultBuildSettings = BuildSettingsVersion.Latest;

		CppStandard = CppStandardVersion.Cpp20;

		bUseRTTI = true;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(PluginDirectory, "Source"),
				Path.Combine(PluginDirectory, "Source/GuideSystem"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/Common"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/STT"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/STT/GuideInteraction"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/STT/GuideThread"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/STE"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/STE/GuideInteraction"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/STE/GuideThread"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/GuideSystem_GameplayTask"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/GuideActor"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/GuideActor/GuideInteraction"),
				Path.Combine(PluginDirectory, "Source/GuideSystem/GuideActor/GuideThread"),
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"UMG",
				"StateTreeModule",
				"AIModule",
				"GameplayStateTreeModule",
				"GameplayTasks",
				"GameplayTags",
				// ... add private dependencies that you statically link with here ...	
				
				"InputCore",
				"Utils", 
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
