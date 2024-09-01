#include <Plugin/Plugin.hpp>

#include <Log/Logger.hpp>

using namespace Ame;

class ExamplePluginHost : public IPlugin
{
public:
    ExamplePluginHost() :
        IPlugin({ "ExamplePluginHost", "WhiteFalcon", "A simple example plugin host", "8/30/2024", { 1, 0, 0, 1 } })
    {
    }

    ~ExamplePluginHost() override
    {
        AME_LOG_INFO("ExamplePluginHost::~ExamplePluginHost");
    }

    bool OnPluginPreLoad(IModuleRegistry* registry) override
    {
        if (IPlugin::OnPluginPreLoad(registry))
        {
            AME_LOG_INFO("ExamplePluginHost::OnPluginPreLoad");
        }
        return true;
    }

    void OnPluginLoad(IModuleRegistry*) override
    {
        AME_LOG_INFO("ExamplePluginHost::OnPluginLoad");
    }

    void OnInterfaceDrop(IObject*) override
    {
        AME_LOG_INFO("ExamplePluginHost::OnInterfaceDrop");
    }

    void OnPluginUnload() override
    {
        AME_LOG_INFO("ExamplePluginHost::OnPluginUnload");
    }

    void OnPluginPauseChange(bool) override
    {
        AME_LOG_INFO("ExamplePluginHost::OnPluginPauseChange");
    }
};

AME_PLUGIN_EXPORT(ExamplePluginHost);