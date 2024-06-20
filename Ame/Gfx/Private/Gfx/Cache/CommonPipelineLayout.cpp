#include <Gfx/Cache/CommonPipelineLayout.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Hash/Layout.hpp>

#include <Gfx/Constants.hpp>

namespace Ame::Gfx::Cache
{
    namespace CD = Constants::DescriptorRanges;

    //

    Co::result<Ptr<Rhi::ScopedPipelineLayout>> CommonPipelineLayout::Load(
        Type type)
    {
        auto Index = std::to_underlying(type);
        if (!m_TypedCaches[Index])
        {
            auto                  executor  = m_Runtime.get().background_executor();
            Co::scoped_async_lock cacheLock = co_await m_Mutex.lock(executor);
            if (!m_TypedCaches[Index])
            {
                m_TypedCaches[Index] = co_await CreateByType(type);
            }
        }
        co_return m_TypedCaches[Index];
    }

    Co::result<Ptr<Rhi::ScopedPipelineLayout>> CommonPipelineLayout::Load(
        const Rhi::PipelineLayoutDesc& layoutDesc)
    {
        auto hash = std::hash<Rhi::PipelineLayoutDesc>{}(layoutDesc);
        auto it   = m_Caches.find(hash);
        if (it != m_Caches.end())
        {
            co_return it->second;
        }

        auto                  executor  = m_Runtime.get().background_executor();
        Co::scoped_async_lock cacheLock = co_await m_Mutex.lock(executor);

        it = m_Caches.find(hash);
        if (it != m_Caches.end())
        {
            co_return it->second;
        }

        auto& resourceAllocator = m_Device.get().GetResourceAllocator();
        auto  pipelineLayout    = std::make_shared<Rhi::ScopedPipelineLayout>(resourceAllocator.CreatePipelineLayout(layoutDesc));

        it = m_Caches.emplace(hash, std::move(pipelineLayout)).first;
        co_return it->second;
    }

    //

    Co::result<Ptr<Rhi::ScopedPipelineLayout>> CommonPipelineLayout::CreateByType(
        Type type)
    {
        auto& resourceAllocator = m_Device.get().GetResourceAllocator();

        switch (type)
        {
        case Type::EntityCollectPass:
        {
            Rhi::DescriptorRangeDesc commandInfo[]{
                { .descriptorNum  = 2,
                  .descriptorType = Rhi::DescriptorType::STORAGE_BUFFER,
                  .shaderStages   = Rhi::ShaderType::COMPUTE_SHADER }
            };

            //

            Rhi::DescriptorSetDesc setDescs[]{
                CD::c_FrameSetDesc<Rhi::ShaderType::COMPUTE_SHADER>,
                CD::c_EntitySetDesc<Rhi::ShaderType::COMPUTE_SHADER>,
                { .registerSpace = 3, .ranges = commandInfo, .rangeNum = Rhi::Count32(commandInfo) },
            };

            Rhi::PushConstantDesc dispatchConstants[]{
                { .size         = Rhi::Size32<uint32_t>() * 3, // contains DrawOffset, DrawCount, CounterOffset
                  .shaderStages = Rhi::ShaderType::COMPUTE_SHADER }
            };

            Rhi::PipelineLayoutDesc layoutDesc{
                .descriptorSets                     = setDescs,
                .pushConstants                      = dispatchConstants,
                .descriptorSetNum                   = Rhi::Count32(setDescs),
                .pushConstantNum                    = Rhi::Count32(dispatchConstants),
                .shaderStages                       = Rhi::ShaderType::COMPUTE_SHADER,
                .enableD3D12DrawParametersEmulation = Rhi::c_EnableDrawParametersEmulation
            };

            co_return std::make_shared<Rhi::ScopedPipelineLayout>(resourceAllocator.CreatePipelineLayout(layoutDesc));
        }
        default:
        {
            std::unreachable();
        }
        }
    }
} // namespace Ame::Gfx::Cache
