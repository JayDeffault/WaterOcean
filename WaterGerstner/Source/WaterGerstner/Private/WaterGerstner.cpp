#include "WaterGerstner.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FWaterGerstnerModule"

void FWaterGerstnerModule::StartupModule()
{
    const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("WaterGerstner"));
    if (Plugin.IsValid())
    {
        const FString ShaderDirectory = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders"));
        AddShaderSourceDirectoryMapping(TEXT("/Plugin/WaterGerstner"), ShaderDirectory);
    }
}

void FWaterGerstnerModule::ShutdownModule()
{
    // Keep global shader mappings intact; Unreal tears plugin mappings down with the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWaterGerstnerModule, WaterGerstner)
