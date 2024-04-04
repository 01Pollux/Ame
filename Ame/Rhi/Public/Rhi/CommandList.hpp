#pragma once

#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    class CommandList
    {
    public:
        CommandList() = default;

        CommandList(
            Device*             RhiDevice,
            nri::CommandBuffer* CommandBuffer) :
            m_RhiDevice(RhiDevice),
            m_CommandBuffer(CommandBuffer)
        {
        }

    private:
        nri::CommandBuffer* m_CommandBuffer = nullptr;
        Device*             m_RhiDevice     = nullptr;
    };
} // namespace Ame::Rhi