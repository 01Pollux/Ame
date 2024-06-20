#include <ranges>

#include <Gfx/Cache/CommonPipelineState.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Hash/Pipeline.hpp>

namespace Ame::Gfx::Cache
{
    Co::result<Ptr<Rhi::ScopedPipelineState>> CommonPipelineState::Load(
        Type type)
    {
        auto index = std::to_underlying(type);
        if (!m_TypedCaches[index])
        {
            auto                  executor  = m_Runtime.get().background_executor();
            Co::scoped_async_lock cacheLock = co_await m_Mutex.lock(executor);
            if (!m_TypedCaches[index])
            {
                auto shaderTask = executor->submit([&]
                                                   { return PrepareShaders(type); });
                auto layoutTask = PrepareLayout(type);

                auto [layoutResult, shadersResult] = co_await Co::when_all(executor, std::move(layoutTask), std::move(shaderTask));

                auto layout  = co_await layoutResult;
                auto shaders = co_await shadersResult;

                m_TypedCaches[index] = CreateByType(*layout, shaders.ShaderDescs, type);
            }
        }
        co_return m_TypedCaches[index];
    }

    Co::result<Ptr<Rhi::ScopedPipelineState>> CommonPipelineState::Load(
        const Rhi::GraphicsPipelineDesc& desc)
    {
        size_t hash = std::hash<Rhi::GraphicsPipelineDesc>{}(desc);
        auto   it   = m_Caches.find(hash);
        if (it != m_Caches.end())
        {
            co_return it->second;
        }

        auto executor = m_Runtime.get().background_executor();
        it            = m_Caches.find(hash);
        if (it != m_Caches.end())
        {
            co_return it->second;
        }

        Co::scoped_async_lock cacheLock = co_await m_Mutex.lock(executor);

        auto& resourceAllcoator = m_Device.get().GetResourceAllocator();
        auto  result            = std::make_shared<Rhi::ScopedPipelineState>(resourceAllcoator.CreatePipelineState(desc));

        it = m_Caches.emplace(hash, result).first;
        co_return result;
    }

    Co::result<Ptr<Rhi::ScopedPipelineState>> CommonPipelineState::Load(
        const Rhi::ComputePipelineDesc& desc)
    {
        size_t hash = std::hash<Rhi::ComputePipelineDesc>{}(desc);
        auto   it   = m_Caches.find(hash);
        if (it != m_Caches.end())
        {
            co_return it->second;
        }

        auto executor = m_Runtime.get().background_executor();
        it            = m_Caches.find(hash);
        if (it != m_Caches.end())
        {
            co_return it->second;
        }

        Co::scoped_async_lock cacheLock = co_await m_Mutex.lock(executor);

        auto& resourceAllcoator = m_Device.get().GetResourceAllocator();
        auto  result            = std::make_shared<Rhi::ScopedPipelineState>(resourceAllcoator.CreatePipelineState(desc));

        it = m_Caches.emplace(hash, result).first;
        co_return result;
    }

    //

    Ptr<Rhi::ScopedPipelineState> CommonPipelineState::CreateByType(
        const Rhi::PipelineLayout& layout,
        const ShaderDescStorage&   shaderDescs,
        Type                       type)
    {
        auto& resourceAllocator = m_Device.get().GetResourceAllocator();

        Ptr<Rhi::ScopedPipelineState> pipelineState;

        switch (type)
        {
        case Type::EntityCollectPass:
        {
            pipelineState = std::make_shared<Rhi::ScopedPipelineState>(
                resourceAllocator.CreatePipelineState(
                    Rhi::ComputePipelineDesc{
                        .Layout = layout.Unwrap(),
                        .Shader = shaderDescs[0] }));
            break;
        }

        default:
            std::unreachable();
        }

        return pipelineState;
    }
} // namespace Ame::Gfx::Cache
