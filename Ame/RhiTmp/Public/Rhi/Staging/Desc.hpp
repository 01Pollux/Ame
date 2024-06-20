#pragma once

#include <Rhi/Descs/Core.hpp>
#include <Rhi/CommandList/CopyDesc.hpp>

namespace Ame::Rhi::Staging
{
    enum class StagedAccessType : uint8_t
    {
        Read,
        Write,
        Count
    };

    static constexpr MemoryLocation StagedAccessToMemoryLocation(
        StagedAccessType accessType)
    {
        switch (accessType)
        {
        case StagedAccessType::Read:
            return MemoryLocation::HOST_READBACK;
        case StagedAccessType::Write:
            return MemoryLocation::HOST_UPLOAD;
        default:
            std::unreachable();
        }
    }

    //

    struct DeferredCopyDesc
    {
        bool PreserveSrcState : 1 = true;
        bool PreserveDstState : 1 = true;
    };

    struct DeferredBufferCopyDesc : BufferCopyDesc, DeferredCopyDesc
    {
    };

    struct DeferredTextureCopyDesc : TextureCopyDesc, DeferredCopyDesc
    {
    };

    struct DeferredTransferCopyDesc : TransferCopyDesc, DeferredCopyDesc
    {
    };
} // namespace Ame::Rhi::Staging