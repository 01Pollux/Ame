#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi::Staging
{
    enum class StagedAccessType : uint8_t
    {
        READ,
        WRITE
    };

    static constexpr MemoryLocation StagedAccessToMemoryLocation(
        StagedAccessType accessType)
    {
        switch (accessType)
        {
        case StagedAccessType::READ:
            return MemoryLocation::HOST_READBACK;
        case StagedAccessType::WRITE:
            return MemoryLocation::HOST_UPLOAD;
        default:
            std::unreachable();
        }
    }

    template<typename PtrTy>
    struct StagedRange
    {
        PtrTy* Ptr = nullptr;
    };

    using StagedReadRange  = StagedRange<const std::byte>;
    using StagedWriteRange = StagedRange<std::byte>;
} // namespace Ame::Rhi::Staging