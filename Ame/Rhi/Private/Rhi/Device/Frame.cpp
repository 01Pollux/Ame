#include <Core/String.hpp>

#include <Rhi/Device/Frame.hpp>
#include <Rhi/Nri/Nri.hpp>

#include <Rhi/NriError.hpp>
#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    void Frame::Shutdown(
        nri::CoreInterface& nriCore)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CommandInfoPools[0].PendingCommands.empty(), "There are still pending graphics command buffers");
        AME_LOG_ASSERT(Log::Rhi(), m_CommandInfoPools[1].PendingCommands.empty(), "There are still pending compute command buffers");
        AME_LOG_ASSERT(Log::Rhi(), m_CommandInfoPools[2].PendingCommands.empty(), "There are still pending copy command buffers");

        for (auto& pools : m_CommandInfoPools)
        {
            for (auto& commandInfo : pools.FreeCommands)
            {
                nriCore.DestroyFence(*commandInfo.NriFence);
                nriCore.DestroyCommandBuffer(*commandInfo.NriCommandBuffer);
                nriCore.DestroyCommandAllocator(*commandInfo.NriCommandAllocator);
            }
        }
    }

    SubmissionContext Frame::AllocateSubmission(
        nri::CoreInterface& nriCore,
        nri::Device&        nriDevice,
        nri::CommandQueue&  nriCommandQueue,
        CommandQueueType    queueType,
        StageBits           stages)
    {
        auto& pools = m_CommandInfoPools[static_cast<uint32_t>(queueType)];

        CommandInfo* commandInfo = nullptr;
        if (pools.FreeCommands.empty())
        {
            commandInfo = &pools.PendingCommands.emplace_back(nriCore, nriDevice, nriCommandQueue, queueType);
        }
        else
        {
            commandInfo = &pools.PendingCommands.emplace_back(pools.FreeCommands.back());
            pools.FreeCommands.pop_back();
        }

        commandInfo->FenceValue++;
        return SubmissionContext{
            .CommandListRef{ &nriCore, commandInfo->NriCommandBuffer },
            .FenceRef{ commandInfo->NriFence, commandInfo->FenceValue, stages },
            .QueueType{ queueType }
        };
    }

    //

    void Frame::NewFrame(
        nri::CoreInterface&     nriCore,
        IDeviceMemoryAllocator& memoryAllocator)
    {
        Release(nriCore, memoryAllocator);
    }

    void Frame::Release(
        nri::CoreInterface&     nriCore,
        IDeviceMemoryAllocator& memoryAllocator)
    {
        for (auto& pool : m_CommandInfoPools)
        {
            std::ranges::move(pool.PendingCommands, std::back_inserter(pool.FreeCommands));
            pool.PendingCommands.clear();
        }

        m_DeferredBuffers.Release(memoryAllocator);
        m_DeferredTextures.Release(memoryAllocator);
        m_DeferredDescriptors.Release(nriCore);
        m_DeferredPipelines.Release(nriCore);
    }

    //

    void Frame::DeferRelease(
        nri::Buffer& nriBuffer)
    {
        m_DeferredBuffers.DeferRelease(nriBuffer);
    }

    void Frame::DeferRelease(
        nri::Texture& nriTexture)
    {
        m_DeferredTextures.DeferRelease(nriTexture);
    }

    void Frame::DeferRelease(
        nri::Descriptor& nriDescriptor)
    {
        m_DeferredDescriptors.DeferRelease(nriDescriptor);
    }

    void Frame::DeferRelease(
        nri::Pipeline& nriPipeline)
    {
        m_DeferredPipelines.DeferRelease(nriPipeline);
    }

    //

    Frame::CommandInfo::CommandInfo(
        nri::CoreInterface& nriCore,
        nri::Device&        nriDevice,
        nri::CommandQueue&  nriCommandQueue,
        CommandQueueType    queueType)
    {
        ThrowIfFailed(
            nriCore.CreateCommandAllocator(nriCommandQueue, NriCommandAllocator),
            "Failed to create command allocator");

        ThrowIfFailed(
            nriCore.CreateCommandBuffer(*NriCommandAllocator, NriCommandBuffer),
            "Failed to create command buffer");

        ThrowIfFailed(
            nriCore.CreateFence(nriDevice, FenceValue, NriFence),
            "Failed to create fence");

#ifndef AME_DIST
        const char* queueName = nullptr;
        switch (queueType)
        {
        case CommandQueueType::GRAPHICS:
            queueName = "Graphics";
            break;
        case CommandQueueType::COMPUTE:
            queueName = "Compute";
            break;
        case CommandQueueType::COPY:
            queueName = "Transfer";
            break;
        default:
            std::unreachable();
        }

        auto commandAllocatorName = std::format("FrameCommandAllocator_{}", queueName);
        auto commandListName      = std::format("FrameCommandList_{}", queueName);
        auto fenceName            = std::format("FrameFence_{}", queueName);

        nriCore.SetCommandAllocatorDebugName(*NriCommandAllocator, commandAllocatorName.c_str());
        nriCore.SetCommandBufferDebugName(*NriCommandBuffer, commandListName.c_str());
        nriCore.SetFenceDebugName(*NriFence, fenceName.c_str());
#endif
    }
} // namespace Ame::Rhi