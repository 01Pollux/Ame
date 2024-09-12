#include <AmeSharp/AmeSharpPlugin.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Interfaces/Scripting/CSharpScriptEngine.hpp>
#include <Log/Logger.hpp>

namespace Ame::Interfaces
{
#ifdef AME_CSHARP_STATIC_LINKING
    Ptr<IScriptEngine> CreateCSharpScriptingEngine();
#endif
} // namespace Ame::Interfaces

namespace Ame::Scripting
{
    AmeSharpPlugin::AmeSharpPlugin() :
        IPlugin({ "AmeSharp", "01Pollux", "C# scripting plugin for Ame Engine", __DATE__ })
    {
    }

    bool AmeSharpPlugin::OnPluginPreLoad(IModuleRegistry* registry)
    {
        if (!IPlugin::OnPluginPreLoad(registry))
        {
            return false;
        }

        try
        {
            auto engine = Interfaces::CreateCSharpScriptingEngine();
            return registry->ExposeInterface(this, Interfaces::IID_CSScriptEngine, engine);
        }
        catch (const std::exception& e)
        {
            AME_LOG_ERROR(std::format("Failed to create C# scripting engine: {}", e.what()));
            return false;
        }
    }
} // namespace Ame::Scripting

AME_PLUGIN_EXPORT(Ame::Scripting::AmeSharpPlugin);