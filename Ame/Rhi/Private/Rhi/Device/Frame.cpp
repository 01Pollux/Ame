#include <Core/String.hpp>

#include <Rhi/Device/Frame.hpp>
#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    void Frame::Initialize(
        DeviceImpl&                     RhiDevice,
        const DescriptorAllocationDesc& DescriptorPoolDesc,
        uint32_t                        FrameIndex)
    {
#ifndef AME_DIST
        auto AllocatorName = StringU8::formatted("FrameCommandAllocator_{}", FrameIndex);
        auto ListName      = StringU8::formatted("FrameCommandList_{}", FrameIndex);

        auto AllocatorNamePtr = AllocatorName.c_str();
        auto ListNamePtr      = ListName.c_str();
#else
        auto AllocatorNamePtr = nullptr;
        auto ListNamePtr      = nullptr;
#endif

        m_CommandList.Initialize(RhiDevice, DescriptorPoolDesc, AllocatorNamePtr, ListNamePtr);
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
        nri::CoreInterface& NriCore,
        MemoryAllocator&    MemAllocator)
    {
        m_CommandList.Reset();
        Release(NriCore, MemAllocator);
    }

    void Frame::EndFrame()
    {
        m_CommandList.End();
    }

    void Frame::Release(
        nri::CoreInterface& NriCore,
        MemoryAllocator&    MemAllocator)
    {
        m_DeferredBuffers.Release(MemAllocator);
        m_DeferredTextures.Release(MemAllocator);
        m_DeferredDescriptors.Release(NriCore);
        m_DeferredPipelines.Release(NriCore);
    }

    //

    void Frame::DeferRelease(
        nri::Buffer& NriBuffer)
    {
        m_DeferredBuffers.DeferRelease(NriBuffer);
    }

    void Frame::DeferRelease(
        nri::Texture& NriTexture)
    {
        m_DeferredTextures.DeferRelease(NriTexture);
    }

    void Frame::DeferRelease(
        nri::Descriptor& NriDescriptor)
    {
        m_DeferredDescriptors.DeferRelease(NriDescriptor);
    }

    void Frame::DeferRelease(
        nri::Pipeline& Pipeline)
    {
        m_DeferredPipelines.DeferRelease(Pipeline);
    }
} // namespace Ame::Rhi