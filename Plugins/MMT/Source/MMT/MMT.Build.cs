
using UnrealBuildTool;
using System.IO;
 
public class MMT : ModuleRules
{
	public MMT(ReadOnlyTargetRules Target) : base (Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;

        CppStandard = CppStandardVersion.Cpp17;

        PrivateIncludePaths.AddRange(new string[] { "MMT/Private" });

        PublicDependencyModuleNames.AddRange(
		new string[] { 
			}
		);
        PrivateDependencyModuleNames.AddRange(
        new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "RHI",
                "RenderCore",
                "UMG",
                "Slate",
                "SlateCore",
                "Chaos",
                "PhysicsCore",
            }
        );
    }
}