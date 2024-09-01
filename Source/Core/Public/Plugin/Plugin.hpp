#pragma once

#include <boost/dll/alias.hpp>
#include <Core/String.hpp>
#include <Core/Version.hpp>
#include <Core/Interface.hpp>

namespace Ame
{
    class IModuleRegistry;

    struct PluginInfo
    {
        const char* Name        = "";
        const char* Author      = "";
        const char* Description = "";
        const char* Date        = "";
        TVersion    Version     = { 1, 0, 0, 0 };
    };

    class IPlugin
    {
        friend class PluginContext;

    public:
        template<typename Ty>
            requires std::derived_from<Ty, IPlugin>
        [[nodiscard]] static IPlugin* Create()
        {
            return new Ty();
        }

    public:
        IPlugin(const PluginInfo& info) noexcept : m_PluginInfo(info)
        {
        }

        virtual ~IPlugin() = default;

        /// <summary>
        /// Called when plugin is first loaded, use this callback to register interfaces only
        /// </summary>
        virtual bool OnPluginPreLoad(IModuleRegistry* registry);

        /// <summary>
        /// Called when plugin is first loaded
        /// </summary>
        /// <returns>
        /// true if plugin is all set to be loaded, false otherwise
        /// </returns>
        virtual void OnPluginLoad(IModuleRegistry*)
        {
        }

        /// <summary>
        /// Called when plugin's interface depdenency is dropped
        /// </summary>
        virtual void OnInterfaceDrop(IObject*)
        {
        }

        /// <summary>
        /// Called when plugin is unloading
        /// </summary>
        virtual void OnPluginUnload()
        {
        }

        /// <summary>
        /// Called when all plugin is changing state
        /// </summary>
        /// <param name="pausing">true if plugin is paused, false otherwise</param>
        virtual void OnPluginPauseChange(bool)
        {
        }

        /// <summary>
        /// Get registered plugin info
        /// </summary>
        [[nodiscard]] const PluginInfo& GetPluginInfo() const noexcept
        {
            return m_PluginInfo;
        }

        /// <summary>
        /// Get plugin state
        /// </summary>
        [[nodiscard]] bool IsPluginPaused() const noexcept
        {
            return m_IsPaused;
        }

        /// <summary>
        /// Set plugin state
        /// </summary>
        void SetPluginState(bool pause) noexcept
        {
            if (pause)
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

            m_IsPaused = pause;
        }

        /// <summary>
        /// Get plugin name
        /// </summary>
        [[nodiscard]] const String& GetPluginName() const noexcept
        {
            return m_PluginName;
        }

    private:
        String           m_PluginName;
        const PluginInfo m_PluginInfo;
        bool             m_IsPaused = false;
    };
} // namespace Ame

#define AME_PLUGIN_EXPORT_FUNCTION Ame_GetPlugin
#define AME_PLUGIN_EXPORT_NAME     "Ame_GetPlugin"
#define AME_PLUGIN_EXPORT(Type)    BOOST_DLL_ALIAS(IPlugin::Create<Type>, AME_PLUGIN_EXPORT_FUNCTION)
