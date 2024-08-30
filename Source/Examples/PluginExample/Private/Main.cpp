#include <Application/EntryPoint.hpp>
#include <Application/Application.hpp>
#include <Engine/Engine.hpp>
#include <Module/Core/PluginHostSubmodule.hpp>

class PluginExampleApplication : public Ame::BaseApplication
{
public:
    explicit PluginExampleApplication(
        const Ame::ApplicationConfig& applicationConfig) :
        BaseApplication(applicationConfig)
    {
    }

    void OnLoad() override
    {
        using namespace Ame;

        Ptr<PluginHostSubmodule> pluginHostSubmodule;
        GetEngine().GetRegistry().GetModule(Ame::IID_CoreModule)->QueryInterface(IID_PluginHostSubmodule, pluginHostSubmodule.DblPtr<IObject>());

        auto pluginHost = pluginHostSubmodule->GetPluginHost();
        pluginHost->LoadPlugin("PluginHost");

        GetEngine().Exit(0);
    }
};

AME_MAIN_APPLICATION_DEFAULT(PluginExampleApplication); 