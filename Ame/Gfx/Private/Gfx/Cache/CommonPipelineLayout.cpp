#include <Gfx/Cache/CommonPipelineLayout.hpp>
#include <Rhi/Device/Device.hpp>

#include <Gfx/Constants.hpp>

namespace Ame::Gfx::Cache
{
    namespace CD = Constants::DescriptorRanges;

    //

    Co::result<Ptr<Rhi::PipelineLayout>> CommonPipelineLayout::Load(
        Type type)
    {
        auto Index = std::to_underlying(type);
        if (!m_Caches[Index])
        {
            auto                  executor  = m_Runtime.get().background_executor();
            Co::scoped_async_lock cacheLock = co_await m_Mutex.lock(executor);
            if (!m_Caches[Index])
            {
                m_Caches[Index] = co_await Create(m_Device, *executor, type);
            }
        }
        co_return m_Caches[Index];
    }

    //

    Co::result<Ptr<Rhi::PipelineLayout>> CommonPipelineLayout::Create(
        Rhi::Device&  Device,
        Co::executor& executor,
        Type          type)
    {
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

            Rhi::PushConstantDesc pushConstants[]{
                { .size         = Rhi::Size32<uint32_t>() * 4, // contains DrawOffset, DrawCount, CounterOffset, _Pad
                  .shaderStages = Rhi::ShaderType::COMPUTE_SHADER }
            };

            Rhi::PipelineLayoutDesc layoutDesc{
                .descriptorSets                     = setDescs,
                .pushConstants                      = pushConstants,
                .descriptorSetNum                   = Rhi::Count32(setDescs),
                .pushConstantNum                    = Rhi::Count32(pushConstants),
                .shaderStages                       = Rhi::ShaderType::COMPUTE_SHADER,
                .enableD3D12DrawParametersEmulation = Rhi::Device::EnableDrawParametersEmulation
            };

            co_return co_await Device.CreatePipelineLayout({}, executor, layoutDesc);
        }
        default:
        {
            std::unreachable();
        }
        }
    }
} // namespace Ame::Gfx::Cache
