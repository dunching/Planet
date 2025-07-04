// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class ItemProxy : ModuleRules
{
	public ItemProxy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		DefaultBuildSettings = BuildSettingsVersion.Latest;

		CppStandard = CppStandardVersion.Cpp20;

		bUseRTTI = true;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
				Path.Combine(PluginDirectory, "Source"),
				Path.Combine(PluginDirectory, "Source/Common"),
				Path.Combine(PluginDirectory, "Source/ItemProxy"),
				Path.Combine(PluginDirectory, "Source/InventoryComponent"),
				Path.Combine(PluginDirectory, "Source/ItemProxyCollection"),
				Path.Combine(PluginDirectory, "Source/ItemDecription_UI"),
				Path.Combine(PluginDirectory, "Source/Visitor"),
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
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
				"GameplayAbilities",
				"NetCore",
				// ... add private dependencies that you statically link with here ...	

				"Utils", 
				"Common_UMG",
				"SceneActor",
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