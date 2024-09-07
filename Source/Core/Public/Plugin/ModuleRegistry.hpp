#pragma once

#include <Plugin/Plugin.hpp>

namespace Ame
{
    class IModuleRegistry
    {
    public:
        virtual ~IModuleRegistry() = default;

        /// <summary>
        /// Expose interface by name.
        /// </summary>
        /// <returns>true if the interface does exists, false otherwise</returns>
        virtual bool ExposeInterface(IPlugin* owner, const UId& iid, IObject* object) = 0;

        /// <summary>
        /// Drop interface by name.
        /// </summary>
        /// <returns>true if the interface does exists, false otherwise</returns>
        virtual bool DropInterface(const UId& iid) = 0;

        /// <summary>
        /// Request an interface by name, and bind it as dependency.
        /// The caller may be null if the interface shouldn't be bound as dependency.
        /// </summary>
        /// <returns>true if the interface does exists, false otherwise</returns>
        virtual bool RequestInterface(IPlugin* caller, const UId& iid, IObject** iface) = 0;

        /// <summary>
        /// find plugin by either it's file name
        /// </summary>
        virtual IPlugin* FindPlugin(const String& name) = 0;

        /// <summary>
        /// find and add plugin to caller's dependencies
        /// </summary>
        /// <param name="bool isRequired">true to immediatly load the plugin if it's not loaded</param>
        /// <returns>true if the plugin, false otherwise</returns>
        virtual IPlugin* BindPlugin(IPlugin* caller, const String& name, bool isRequired) = 0;

        /// <summary>
        /// Get plugin host version.
        /// </summary>
        [[nodiscard]] virtual TVersion GetHostVersion() = 0;

        /// <summary>
        /// Load and init a plugin outside of its host.
        /// </summary>
        /// <returns>true if the plugin was successfully loaded, false otherwise</returns>
        virtual IPlugin* LoadPlugin(const String& name) = 0;

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

    /// <summary>
    /// Create a new module registry.
    /// </summary>
    UniquePtr<IModuleRegistry> CreateModuleRegistry();
} // namespace Ame