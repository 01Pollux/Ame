#include <AmeSharp/AmeSharpPlugin.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Interfaces/Scripting/CSharpScriptEngine.hpp>
#include <Log/Logger.hpp>

#include <Scripting/Library.hpp>
#include <Scripting/Type.hpp>
#include <Scripting/Method.hpp>

namespace Ame::Scripting
{
    static void Test(Interfaces::IScriptEngine* engine)
    {
        auto library = engine->CreateLibrary("ExampleContext", "Plugins/Sample.dll");
        auto name    = library->GetName();

        AME_LOG_INFO(std::format("Library name: {}", library->GetName().view()));
        for (auto& type : library->GetTypes())
        {
            AME_LOG_INFO(std::format("Type: {}", type->GetName().view()));
        }

        auto type = library->GetType("Example.Managed.ExampleClass");

        NativeArray<int>              arr{ 1, 2, 3, 4, 5 };
        NativeArray<int>              arr2{ 1, 2, 3, 4, 5 };
        NativeArray<NativeArray<int>> arr3{
            NativeArray<int>{ 1, 2, 3, 4, 5 },
            NativeArray<int>{ 5, 4, 3, 2, 1 },
        };
        NativeArray<NativeString> arr4{ "Testing", "Testing2" };
        int sum = type->GetMethod("SumArray")->InvokeStatic<int>(arr, arr4, "Testing2", arr2, arr3);
        printf("Sum: %d\n", sum);
    }

    //

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
            Test(engine);
            return registry->ExposeInterface(this, Interfaces::IID_CSScriptEngine, engine);
        }
        catch (const std::exception& e)
        {
            AME_LOG_ERROR(std::format("Failed to create C# scripting engine: {}", e.what()));
            return false;
        }
    }
} // namespace Ame::Scripting

AME_PLUGIN_EXPORT(Ame::Scripting::AmeSharpRuntimePlugin);