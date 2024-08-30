#include <Application/EntryPoint.hpp>
#include <Application/Application.hpp>

class PluginExampleApplication : public Ame::BaseApplication
{
public:
    explicit PluginExampleApplication(
        const Ame::ApplicationConfig& applicationConfig) :
        BaseApplication(applicationConfig)
    {
    }
};

AME_MAIN_APPLICATION_DEFAULT(PluginExampleApplication);