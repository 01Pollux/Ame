#include <Plugin/Plugin.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Interfaces/Core/Logger.hpp>

namespace Ame
{
    IPlugin::IPlugin(const PluginInfo& info) noexcept : m_PluginInfo(info)
    {
    }

    void IPlugin::SetPluginState(bool paused)
    {
        if (paused)
        {
            if (!m_IsPaused)
            {
                this->OnPluginPauseChange(true);
            }
        }
        else if (m_IsPaused)
        {
            this->OnPluginPauseChange(false);
        }

        m_IsPaused = paused;
    }
} // namespace Ame