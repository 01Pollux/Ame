#pragma once

#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    static constexpr nri::SPIRVBindingOffsets c_DefaultSpirvBindingOffset{
        .samplerOffset                 = 1000,
        .textureOffset                 = 2000,
        .constantBufferOffset          = 3000,
        .storageTextureAndBufferOffset = 4000
    };
} // namespace Ame::Rhi