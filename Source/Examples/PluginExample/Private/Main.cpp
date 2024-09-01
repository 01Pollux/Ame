#include <Application/EntryPoint.hpp>
#include <Application/Application.hpp>
#include <Engine/Engine.hpp>

class PluginExampleApplication : public Ame::BaseApplication
{
public:
    explicit PluginExampleApplication(const Ame::ApplicationConfig& applicationConfig) :
        BaseApplication(applicationConfig)
    {
    }

    void OnLoad() override
    {
        auto moduleRegistry = GetEngine().GetRegistry();
        moduleRegistry->LoadPlugin("PluginHost");

        GetEngine().Exit(0);
    }
};

AME_MAIN_APPLICATION_DEFAULT(PluginExampleApplication, "Logs/PluginExampleApplication.log");