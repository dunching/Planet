// Copyright Ben Sutherland 2024. All rights reserved.

using UnrealBuildTool;

public class FlyingNavSystem : ModuleRules
{
	public FlyingNavSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        
#if UE_5_3_OR_LATER
        PublicDefinitions.Add("UE_INCLUDE_ORDER_5_3");
#elif UE_5_2_OR_LATER
        PublicDefinitions.Add("UE_INCLUDE_ORDER_5_2");
#elif UE_5_1_OR_LATER
        PublicDefinitions.Add("UE_INCLUDE_ORDER_5_1");
#endif

		PublicDependencyModuleNames.AddRange(new[] {"Core", "CoreUObject", "Engine", "NavigationSystem"}); 
		PrivateDependencyModuleNames.AddRange(new[] {"RHI", "RenderCore", "AIModule"}); // RHI: FDynamicPrimitiveUniformBuffer, RenderCore: FIndexBuffer and FVertexFactory

		PublicIncludePathModuleNames.AddRange(new[] {"AIModule"}); // AIModule has GraphAStar.h, which is header-only.

		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(new[] {"UnrealEd"});
		}
		
		PublicDefinitions.Add(Target.Platform.ToString() == "PS5" ? "PLATFORM_PS5=1" : "PLATFORM_PS5=0");
	}
}
