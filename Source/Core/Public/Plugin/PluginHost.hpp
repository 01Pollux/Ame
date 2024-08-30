#pragma once

#include <Plugin/Plugin.hpp>

namespace Ame
{
    class IPluginHost
    {
    public:
        /// <summary>
        /// Expose interface by name.
        /// </summary>
        /// <returns>true if the interface does exists, false otherwise</returns>
        virtual bool ExposeInterface(const UId& iid, IObject* object, IPlugin* owner) = 0;

        /// <summary>
        /// Request an interface by name.
        /// </summary>
        /// <returns>true if the interface does exists, false otherwise</returns>
        virtual bool RequestInterface(const UId& iid, IObject** iface) = 0;

        /// <summary>
        /// find plugin by either it's file name
        /// </summary>
        [[nodiscard]] virtual IPlugin* FindPlugin(const String& name) = 0;

        /// <summary>
        /// find and add plugin to caller's dependencies
        /// </summary>
        /// <param name="bool isRequired">true to immediatly load the plugin if it's not loaded</param>
        /// <returns>true if the plugin, false otherwise</returns>
        virtual IPlugin* BindPlugin(IPlugin* caller, const String& name, bool isRequired) = 0;

        /// <summary>
        /// Unload main plugin and all of its subplugins.
        /// </summary>
        virtual void Shutdown() = 0;

        /// <summary>
        /// Get plugin host version.
        /// </summary>
        [[nodiscard]] virtual TVersion GetHostVersion() = 0;

        /// <summary>
        /// Load and init a plugin outside of its host.
        /// </summary>
        /// <returns>true if the plugin was successfully loaded, false otherwise</returns>
        [[nodiscard]] virtual IPlugin* LoadPlugin(const String& name) = 0;

        /// <summary>
        /// force unload the plugin.
        /// </summary>
        virtual bool UnloadPlugin(const String& name) = 0;

        /// <summary>
        /// force unload the plugin.
        /// </summary>
        bool UnloadPlugin(IPlugin* plugin)
        {
            return UnloadPlugin(plugin->GetPluginName());
        }
    };
} // namespace Ame