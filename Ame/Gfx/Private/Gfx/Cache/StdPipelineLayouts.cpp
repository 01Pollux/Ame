#include <Gfx/Cache/StdPipelineLayouts.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx::Cache
{
    Co::result<Ptr<Rhi::PipelineLayout>> PipelineLayoutCache::Get(
        Type LayoutType)
    {
        auto Index = std::to_underlying(LayoutType);
        if (!m_Caches[Index])
        {
            auto Executor = m_Runtime.get().background_executor();

            Co::scoped_async_lock Lock = co_await m_Mutex.lock(Executor);
            if (!m_Caches[Index])
            {
                auto Desc       = GetDesc(LayoutType);
                m_Caches[Index] = co_await m_Device.get().CreatePipelineLayout({}, *Executor, Desc);
            }
        }
        co_return m_Caches[Index];
    }

    //

    Rhi::PipelineLayoutDesc PipelineLayoutCache::GetDesc(
        Type LayoutType)
    {
        switch (LayoutType)
        {
        case Type::EntityCollectPass:
        {

            break;
        }
        default:
        {
            std::unreachable();
        }
        }
    }
} // namespace Ame::Gfx::Cache
