// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class Common_UMG : ModuleRules
{
	public Common_UMG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		DefaultBuildSettings = BuildSettingsVersion.Latest;

		UndefinedIdentifierWarningLevel = WarningLevel.Error;
		bWarningsAsErrors = true;
		CppStandard = CppStandardVersion.Cpp20;

		bUseRTTI = true;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(PluginDirectory, "Source/WidgetScreenLayer"),
				Path.Combine(PluginDirectory, "Source/ScaleableWidget"),
				Path.Combine(PluginDirectory, "Source/Common_UMG"),
				Path.Combine(PluginDirectory, "Source/Common_UMG/RichTextBlock"),
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
