#include <AmeSharp/AmeSharpPlugin.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Interfaces/Scripting/CSharpScriptEngine.hpp>
#include <Log/Logger.hpp>

#include <Scripting/Library.hpp>

namespace Ame::Scripting
{
    AmeSharpRuntimePlugin::AmeSharpRuntimePlugin() :
        IPlugin({ "AmeSharp.Runtime", "01Pollux", "C# scripting plugin for Ame Engine", __DATE__ })
    {
    }

    bool AmeSharpRuntimePlugin::OnPluginPreLoad(IModuleRegistry* registry)
    {
        if (!IPlugin::OnPluginPreLoad(registry))
        {
            return false;
        }

        try
        {
            CSScriptEngineConfig scriptConfig{ .RuntimeConfigPath = GetPluginPath() };

            auto engine = Interfaces::CreateCSharpScriptingEngine(scriptConfig);
            bool ret    = registry->ExposeInterface(this, Interfaces::IID_CSScriptEngine, engine);

            //

            {
                auto library =
                    engine->CreateLibrary(NativeString("ExampleContext"), NativeString("Plugins/Sample.dll"));
                auto name = library->GetName();
                (void)library;
            }

            //

            return ret;
        }
        catch (const std::exception& e)
        {
            AME_LOG_ERROR(std::format("Failed to create C# scripting engine: {}", e.what()));
            return false;
        }
    }
} // namespace Ame::Scripting

AME_PLUGIN_EXPORT(Ame::Scripting::AmeSharpRuntimePlugin);