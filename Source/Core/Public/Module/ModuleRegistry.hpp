#pragma once

#include <unordered_map>
#include <Core/Interface.hpp>
#include <Module/Module.hpp>

namespace Ame
{
    class IModule;

    /// <summary>
    /// Modules are DLLs that are loaded at runtime, they may also be statically linked.
    /// Modules are used to separate the engine into smaller parts.
    /// Modules are lazy loaded, meaning they are only loaded when they are needed by calling LoadModule() then GetModule().
    ///
    /// Core Module:
    ///     TimeSubmodule
    ///     CoroutineSubmodule
    ///     FrameEventSubmodule (OnFrameStart, OnFrameEnd)
    ///     StateSubmodule
    /// Plugin Module:
    ///     PluginManagerSubmodule
    ///     PluginEventSubmodule (OnPluginLoad, OnPluginUnload, OnPluginReload)
    ///     PluginLoaderSubmodule
    ///     PluginLanguageBridgeSubmodule
    /// Asset Module:
    ///     ResourceManagerSubmodule
    ///     ResourcePackagerSubmodule
    ///     PackageConverterSubmodule
    /// Network Module:
    ///     WebSubmodule
    ///     LocalSubmodule
    ///     ServerSubmodule
    ///     ClientSubmodule
    ///     DatabaseSubmodule
    /// System Module:
    ///     RhiSubmodule
    ///     WindowSubmodule
    ///     InputSubmodule
    /// Audio Module:
    ///     SoundSubmodule
    ///     VoiceSubmodule
    ///     MixerSubmodule
    /// Graphics Module:
    ///     RendererSubmodule
    ///     UISubmodule
    ///     ImGUISubmodule
    /// Script Module:
    ///     LuaSubmodule
    ///     CSharpScriptSubmodule
    /// </summary>
    class ModuleRegistry
    {
    public:
        /// <summary>
        /// Register a preloaded module
        /// </summary>
        template<typename Ty, typename... Args>
        void RegisterModule(
            Args&&... args)
        {
            RegisterModule({ ObjectAllocator<Ty>()(std::forward<Args>(args)...), IID_BaseModule });
        }

        /// <summary>
        /// Register a preloaded module
        /// </summary>
        void RegisterModule(
            Ptr<IModule> module);

        /// <summary>
        /// Unload a module by id
        /// </summary>
        void UnregisterModule(
            const UId& moduleId);

        /// <summary>
        /// Check if a module is loaded by id
        /// </summary>
        /// <param name="name">The name of the module</param>
        /// <returns>True if the module is loaded, false otherwise</returns>
        [[nodiscard]] bool ContainsModule(
            const UId& moduleId) const;

        /// <summary>
        /// Get a module by id
        /// </summary>
        /// <param name="name">The name of the module</param>
        /// <returns>The module if it is loaded, nullptr otherwise</returns>
        [[nodiscard]] Ptr<IModule> GetModule(
            const UId& moduleId) const;

    private:
        using ModuleMap = std::unordered_map<UId, Ptr<IModule>, UIdHasher>;

    private:
        ModuleMap m_Modules;
    };
} // namespace Ame