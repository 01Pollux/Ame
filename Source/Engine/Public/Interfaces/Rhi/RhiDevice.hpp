#pragma once

#include <Core/Interface.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Interfaces
{
    inline constexpr UId IID_RhiDevice = Rhi::IID_RhiDevice;
    using IRhiDevice                   = Rhi::IRhiDevice;
} // namespace Ame::Interfaces