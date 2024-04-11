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
        nri::CoreInterface& NriCore)
    {
        m_CommandList.Reset(NriCore);
    }

    void Frame::EndFrame(
        nri::CoreInterface& NriCore)
    {
        m_CommandList.End(NriCore);
    }
} // namespace Ame::Rhi