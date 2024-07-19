#include <Module/ModuleRegistry.hpp>
#include <Module/Module.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    void ModuleRegistry::RegisterModule(
        Ptr<IModule> module)
    {
        AME_LOG_ASSERT(Log::Engine(), module != nullptr, "Module is nullptr");
        AME_LOG_ASSERT(Log::Engine(), !ContainsModule(module->GetId()), "Module already exists");

        module->Initialize();
        m_Modules[module->GetId()] = module;
    }

    void ModuleRegistry::UnregisterModule(
        const UId& moduleId)
    {
        AME_LOG_ASSERT(Log::Engine(), ContainsModule(moduleId), "Module does not exist");

        auto moduleIter = m_Modules.find(moduleId);
        moduleIter->second->Shutdown();
        m_Modules.erase(moduleIter);
    }

    bool ModuleRegistry::ContainsModule(
        const UId& moduleId) const
    {
        return m_Modules.contains(moduleId);
    }

    Ptr<IModule> ModuleRegistry::GetModule(
        const UId& moduleId) const
    {
        return m_Modules.at(moduleId);
    }
} // namespace Ame
