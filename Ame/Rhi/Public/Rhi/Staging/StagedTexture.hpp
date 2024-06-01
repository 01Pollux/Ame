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
        [[nodiscard]] const TextureDesc& GetTextureDesc() const;

        [[nodiscard]] const Buffer& GetBuffer() const;

        [[nodiscard]] size_t GetBufferSize() const;

        [[nodiscard]] BufferRange GetRegion(
            uint32_t    mipLevel   = 0,
            uint32_t    arrayIndex = 0,
            TextureRect rect       = c_EntireTexture) const;

    private:
        Ref<const DeviceDesc> m_DeviceDesc;
        TextureDesc           m_Desc;
        size_t                m_TextureSize = 0;
        Buffer                m_TextureBuffer;
    };
} // namespace Ame::Rhi::Staging