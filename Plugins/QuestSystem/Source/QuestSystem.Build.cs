// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class QuestSystem : ModuleRules
{
	public QuestSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		DefaultBuildSettings = BuildSettingsVersion.Latest;

		CppStandard = CppStandardVersion.Cpp20;

		bUseRTTI = true;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(PluginDirectory, "Source"),
				Path.Combine(PluginDirectory, "Source/QuestSystem"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/Common"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/STT"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/STT/Interaction"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/STT/Chain"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/STE"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/STE/Interaction"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/STE/Chain"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/QuesSystem_GameplayTask"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/QuestActor"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/QuestActor/Interaction"),
				Path.Combine(PluginDirectory, "Source/QuestSystem/QuestActor/Chain"),
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
