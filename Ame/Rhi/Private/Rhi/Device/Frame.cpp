#include <Core/String.hpp>

#include <Rhi/Device/Frame.hpp>
#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    void Frame::Initialize(
        nri::CoreInterface& NriCore,
        nri::CommandQueue&  GraphicsQueue,
        uint32_t            FrameIndex)
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

        m_CommandList.Initialize(NriCore, GraphicsQueue, AllocatorNamePtr, ListNamePtr);
    }

    void Frame::Shutdown(
        nri::CoreInterface& NriCore)
    {
        m_CommandList.Shutdown(NriCore);
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
        m_CommandList.Reset(NriCore);
        Release(NriCore, MemAllocator);
    }

    void Frame::EndFrame(
        nri::CoreInterface& NriCore)
    {
        m_CommandList.End(NriCore);
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