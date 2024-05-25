#include <ranges>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/CommonPipelineState.hpp>

namespace Ame::Gfx::Cache
{
    Co::result<Ptr<Rhi::PipelineState>> CommonPipelineState::Load(
        Type type)
    {
        auto index = std::to_underlying(type);
        if (!m_Caches[index])
        {
            auto shaderTasks = PrepareShaders(type);
            auto layoutTask  = PrepareLayout(type);

            auto                  executor  = m_Runtime.get().background_executor();
            Co::scoped_async_lock cacheLock = co_await m_Mutex.lock(executor);
            if (!m_Caches[index])
            {
                auto Layout     = co_await layoutTask;
                auto Shaders    = co_await ShaderTable::Create(co_await shaderTasks);
                m_Caches[index] = co_await Create(m_Device, *executor, Layout, Shaders.ShaderDescs, type);
            }
        }
        co_return m_Caches[index];
    }

    //

    Co::result<Ptr<Rhi::PipelineState>> CommonPipelineState::Create(
        Rhi::Device&                    rhiDevice,
        Co::executor&                   executor,
        const Ptr<Rhi::PipelineLayout>& layout,
        const ShaderDescStorage&        shaderDescs,
        Type                            type)
    {
        switch (type)
        {
        case Type::EntityCollectPass:
        {
            return rhiDevice.CreatePipelineState(
                {},
                executor,
                Rhi::ComputePipelineDesc{
                    .Layout = layout,
                    .Shader = shaderDescs[0] });
            break;
        }

        default:
            std::unreachable();
        }
    }
} // namespace Ame::Gfx::Cache
