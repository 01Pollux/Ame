#include <Gfx/Cache/PipelineLayoutCache.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx::Cache
{
    Co::result<Ptr<Rhi::PipelineLayout>> PipelineLayoutCache::Load(
        Type LayoutType)
    {
        auto Index = std::to_underlying(LayoutType);
        if (!m_Caches[Index])
        {
            auto                  Executor = m_Runtime.get().background_executor();
            Co::scoped_async_lock Lock     = co_await m_Mutex.lock(Executor);
            if (!m_Caches[Index])
            {
                m_Caches[Index] = co_await Create(m_Device, *Executor, LayoutType);
            }
        }
        co_return m_Caches[Index];
    }

    //

    Co::result<Ptr<Rhi::PipelineLayout>> PipelineLayoutCache::Create(
        Rhi::Device&  Device,
        Co::executor& Executor,
        Type          LayoutType)
    {
        switch (LayoutType)
        {
        case Type::EntityCollectPass:
        {
            Rhi::DescriptorRangeDesc FrameDescriptor[]{
                { .descriptorNum  = 1,
                  .descriptorType = Rhi::DescriptorType::CONSTANT_BUFFER,
                  .shaderStages   = Rhi::ShaderBits::COMPUTE_SHADER }
            };

            Rhi::DescriptorRangeDesc EntityData[]{
                { .descriptorNum  = 2,
                  .descriptorType = Rhi::DescriptorType::STRUCTURED_BUFFER,
                  .shaderStages   = Rhi::ShaderBits::COMPUTE_SHADER }
            };

            Rhi::DescriptorRangeDesc CommandInfo[]{
                { .descriptorNum  = 2,
                  .descriptorType = Rhi::DescriptorType::STORAGE_BUFFER,
                  .shaderStages   = Rhi::ShaderBits::COMPUTE_SHADER }
            };

            //

            Rhi::DescriptorSetDesc Sets[]{
                { .registerSpace = 0, .ranges = FrameDescriptor, .rangeNum = Rhi::Count32(FrameDescriptor) },
                { .registerSpace = 1, .ranges = EntityData, .rangeNum = Rhi::Count32(EntityData) },
                { .registerSpace = 2, .ranges = CommandInfo, .rangeNum = Rhi::Count32(CommandInfo) },
            };

            Rhi::PushConstantDesc PushConstants[]{
                { .size = Device.GetDrawIndexedCommandSize(), .shaderStages = Rhi::ShaderBits::COMPUTE_SHADER }
            };

            Rhi::PipelineLayoutDesc Desc{
                .descriptorSets   = Sets,
                .pushConstants    = PushConstants,
                .descriptorSetNum = Rhi::Count32(Sets),
                .pushConstantNum  = Rhi::Count32(PushConstants),
                .shaderStages     = Rhi::ShaderBits::COMPUTE_SHADER
            };

            co_return co_await Device.CreatePipelineLayout({}, Executor, Desc);
        }
        default:
        {
            std::unreachable();
        }
        }
    }
} // namespace Ame::Gfx::Cache
