#include <ranges>

#include <Gfx/Cache/PipelineStateCache.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx::Cache
{
    Co::result<Ptr<Rhi::PipelineState>> PipelineStateCache::Load(
        Type PipelineType)
    {
        auto Index = std::to_underlying(PipelineType);
        if (!m_Caches[Index])
        {
            auto ShaderTasks = PrepareShaders(PipelineType);
            auto LayoutTask  = PrepareLayout(PipelineType);

            auto                  Executor = m_Runtime.get().background_executor();
            Co::scoped_async_lock Lock     = co_await m_Mutex.lock(Executor);
            if (!m_Caches[Index])
            {
                auto Layout     = co_await LayoutTask;
                auto Shaders    = co_await ShaderTable::Create(co_await ShaderTasks);
                m_Caches[Index] = co_await Create(m_Device, *Executor, Layout, Shaders.ShaderDescs, PipelineType);
            }
        }
        co_return m_Caches[Index];
    }

    //

    Co::result<Ptr<Rhi::PipelineState>> PipelineStateCache::Create(
        Rhi::Device&                    Device,
        Co::executor&                   Executor,
        const Ptr<Rhi::PipelineLayout>& Layout,
        const ShaderDescStorage&        ShaderDescs,
        Type                            PipelineType)
    {
        switch (PipelineType)
        {
        case Type::EntityCollectPass:
        {
            return Device.CreatePipelineState(
                {},
                Executor,
                Rhi::ComputePipelineDesc{
                    .Layout = Layout,
                    .Shader = ShaderDescs[0] });
            break;
        }

        default:
            std::unreachable();
        }
    }
} // namespace Ame::Gfx::Cache
