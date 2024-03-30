#include "Frame.hpp"
#include <NRIDescs.h>

#include "../NriError.hpp"

namespace Ame::Rhi
{
    void Frame::Initialize(
        nri::CoreInterface& NriCore,
        nri::CommandQueue&  GraphicsQueue)
    {
        ThrowIfFailed(NriCore.CreateCommandAllocator(GraphicsQueue, m_CommandAllocator), "Failed to create command allocator");
        ThrowIfFailed(NriCore.CreateCommandBuffer(*m_CommandAllocator, m_CommandBuffer), "Failed to create command buffer");
    }

    void Frame::Shutdown(
        nri::CoreInterface& NriCore)
    {
        NriCore.DestroyCommandBuffer(*m_CommandBuffer);
        NriCore.DestroyCommandAllocator(*m_CommandAllocator);
    }
    //

    void Frame::NewFrame(
        nri::CoreInterface& NriCore)
    {
        NriCore.ResetCommandAllocator(*m_CommandAllocator);
        NriCore.BeginCommandBuffer(*m_CommandBuffer, nullptr);
    }

    nri::CommandBuffer* Frame::EndFrame(
        nri::CoreInterface& NriCore)
    {
        NriCore.EndCommandBuffer(*m_CommandBuffer);
        return m_CommandBuffer;
    }
} // namespace Ame::Rhi