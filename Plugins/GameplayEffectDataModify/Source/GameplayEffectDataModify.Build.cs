// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class GameplayEffectDataModify : ModuleRules
{
	public GameplayEffectDataModify(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		DefaultBuildSettings = BuildSettingsVersion.Latest;

		CppStandard = CppStandardVersion.Cpp20;

		bUseRTTI = true;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(PluginDirectory, "Source"),
				Path.Combine(PluginDirectory, "Source/Common"),
				Path.Combine(PluginDirectory, "Source/GameplayEffectDataModify"),
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
				"GameplayTags",
				"GameplayAbilities",
				// ... add private dependencies that you statically link with here ...	
				
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
