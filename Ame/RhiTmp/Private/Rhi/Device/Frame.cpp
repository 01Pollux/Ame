#include <Core/String.hpp>

#include <Rhi/Device/Frame.hpp>
#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    void Frame::Initialize(
        DeviceImpl&                     rhiDevice,
        const DescriptorAllocationDesc& descriptorPoolDesc,
        uint32_t                        frameIndex)
    {
#ifndef AME_DIST
        auto commandAllocatorName = std::format("FrameCommandAllocator_{}", frameIndex);
        auto commandListName      = std::format("FrameCommandList_{}", frameIndex);

        auto commandAllocatorNamePtr = commandAllocatorName.c_str();
        auto commandListNamePtr      = commandListName.c_str();
#else
        auto commandAllocatorNamePtr = nullptr;
        auto commandListNamePtr      = nullptr;
#endif

        m_CommandList.Initialize(rhiDevice, descriptorPoolDesc, commandAllocatorNamePtr, commandListNamePtr);
    }

    void Frame::Shutdown()
    {
        m_CommandList.Shutdown();
    }

    //

    CommandListImpl& Frame::GetCommandList() noexcept
    {
        return m_CommandList;
    }

    //

    void Frame::NewFrame(
        nri::CoreInterface& nriCore,
        MemoryAllocator&    memoryAllocator)
    {
        m_CommandList.Reset();
        Release(nriCore, memoryAllocator);
    }

    void Frame::EndFrame()
    {
        m_CommandList.End();
    }

    void Frame::Release(
        nri::CoreInterface& nriCore,
        MemoryAllocator&    memoryAllocator)
    {
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
} // namespace Ame::Rhi