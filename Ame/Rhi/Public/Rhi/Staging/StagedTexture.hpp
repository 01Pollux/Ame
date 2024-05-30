#pragma once

#include <Rhi/Staging/Desc.hpp>
#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Rhi::Staging
{
    class StagedTexture
    {
    public:
        StagedTexture(
            Device&            rhiDevice,
            const TextureDesc& desc,
            StagedAccessType   accessType);

    public:
        [[nodiscard]] BufferRange GetRegion(
            uint32_t                mipLevel   = 0,
            uint32_t                arrayIndex = 0,
            TextureRect::Coordinate position   = { 0, 0, 0 }) const;

    private:
        Ref<const DeviceDesc> m_DeviceDesc;
        TextureDesc           m_Desc;
        Buffer                m_TextureBuffer;
    };
} // namespace Ame::Rhi::Staging