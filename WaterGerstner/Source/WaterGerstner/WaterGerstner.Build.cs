using UnrealBuildTool;

public class WaterGerstner : ModuleRules
{
    public WaterGerstner(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "PhysicsCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Projects",
            "RenderCore",
            "RHI"
        });
    }
}
