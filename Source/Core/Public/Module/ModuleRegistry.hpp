#pragma once

#include <unordered_map>
#include <Core/Interface.hpp>
#include <Module/Module.hpp>

namespace Ame
{
    class IModule;

    /// <summary>
    /// Modules are used to separate the engine into smaller parts.
    ///
    /// Core Module:
    ///     TimeSubmodule
    ///     CoroutineSubmodule
    ///     FrameEventSubmodule (OnFrameStart, OnFrameUpdate, OnFrameEnd)
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
    /// Rhi Module:
    ///     PlatformWindowSubmodule
    ///     RhiGraphicsSubmodule
    ///     ImGuiSubmodule
    /// Graphics Module:
    ///     RendererSubmodule
    ///     UISubmodule
    /// Script Module:
    ///     LuaSubmodule
    ///     CSharpScriptSubmodule
    /// World Entity Module:
    ///     EntityStorageSubmodule
    ///     EntityComponentSubmodule
    ///     EntityPrefabSubmodule
    /// Editor Module:
    ///     EditorWindowSubmodule
    /// </summary>
    class ModuleRegistry
    {
    public:
        /// <summary>
        /// Register a preloaded module
        /// </summary>
        template<typename Ty, typename... Args>
        Ty* RegisterModule(
            Args&&... args)
        {
            Ptr<Ty> module{ ObjectAllocator<Ty>()(std::forward<Args>(args)...) };
            RegisterModule(module.Cast<IModule>(IID_BaseModule));
            return module;
        }

        /// <summary>
        /// Register a preloaded module
        /// </summary>
        IModule* RegisterModule(
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
        [[nodiscard]] const Ptr<IModule>& GetModule(
            const UId& moduleId) const;

        /// <summary>
        /// Get a module by id
        /// </summary>
        /// <param name="name">The name of the module</param>
        /// <returns>The module if it is loaded, nullptr otherwise</returns>
        template<typename Ty>
        [[nodiscard]] Ptr<Ty> GetModule(
            const UId& moduleId) const
        {
            return GetModule(moduleId).Cast<Ty>(moduleId);
        }

    private:
        using ModuleMap = std::unordered_map<UId, Ptr<IModule>, UIdUtils::Hasher>;

    private:
        ModuleMap m_Modules;
    };
} // namespace Ame