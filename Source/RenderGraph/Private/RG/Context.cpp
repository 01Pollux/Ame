#include <RG/Context.hpp>

namespace Ame::RG
{
    const ResourceStorage& Context::GetStorage() const
    {
        return m_Resources;
    }

    ResourceStorage& Context::GetStorage()
    {
        return m_Resources;
    }

    //

    void Context::Execute(
        Dg::IRenderDevice*             renderDevice,
        std::span<Dg::IDeviceContext*> contexts)
    {
        GetStorage().UpdateResources(renderDevice);
        for (auto& level : m_Levels)
        {
            level.Execute(*this, contexts);
        }
    }

    void Context::Build(
        DependencyLevelListType&& levels)
    {
        m_Levels = std::move(levels);
    }
} // namespace Ame::RG