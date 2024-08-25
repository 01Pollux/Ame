#include <Rg/Context.hpp>

namespace Ame::Rg
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
        Dg::IDeviceContext* context)
    {
        GetStorage().UpdateResources();
        for (auto& level : m_Levels)
        {
            level.Execute(*this, context);
        }
    }

    void Context::Build(
        DependencyLevelListType&& levels)
    {
        m_Levels = std::move(levels);
    }
} // namespace Ame::Rg