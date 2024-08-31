#include <Plugin/Plugin.hpp>

#include <Log/Wrapper.hpp>

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
        Log::Engine().Info("ExamplePluginHost::~ExamplePluginHost");
    }

    void OnPluginPreLoad(IModuleRegistry*) override
    {
        Log::Engine().Info("ExamplePluginHost::OnPluginPreLoad");
    }

    bool OnPluginLoad(IModuleRegistry*) override
    {
        Log::Engine().Info("ExamplePluginHost::OnPluginLoad");
        return true;
    }

    void OnInterfaceDrop(IObject*) override
    {
        Log::Engine().Info("ExamplePluginHost::OnInterfaceDrop");
    }

    void OnPluginUnload() override
    {
        Log::Engine().Info("ExamplePluginHost::OnPluginUnload");
    }

    void OnPluginPauseChange(bool) override
    {
        Log::Engine().Info("ExamplePluginHost::OnPluginPauseChange");
    }
};

AME_PLUGIN_EXPORT(ExamplePluginHost);