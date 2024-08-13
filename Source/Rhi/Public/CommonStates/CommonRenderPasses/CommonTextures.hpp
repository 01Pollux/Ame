#pragma once

#include <map>
#include <Rhi/Core.hpp>
#include <Math/Vector.hpp>

namespace Ame::Rhi
{
    enum class CommonTexture : uint8_t
    {
        White1D,
        White2D,
        White3D,
        WhiteCube,

        Black1D,
        Black2D,
        Black3D,
        BlackCube,

        Grey1D,
        Grey2D,
        Grey3D,
        GreyCube,

        Checkboard,
        DevTexture,

        Count
    };

    /// <summary>
    /// All common render passes can use cached render pass.
    /// </summary>s
    class CommonTextures
    {
        using DefaultTextureList = std::array<Ptr<Dg::ITexture>, std::to_underlying(CommonTexture::Count)>;
        using CustomTextureList  = std::map<size_t, Ptr<Dg::ITexture>>;

    public:
        CommonTextures(
            IRhiDevice* rhiDevice);

    public:
        [[nodiscard]] Dg::ITexture* GetDefaultTexture(
            CommonTexture type);

        [[nodiscard]] Dg::ITexture* GetCheckboardTexture(
            const Math::Vector2U& size   = { 64, 64 },
            const Math::Vector2U& slices = { 8, 8 });

        [[nodiscard]] Dg::ITexture* GetDevTexture(
            const Math::Vector2U& size = { 64, 64 });

    private:
        IRhiDevice* m_RhiDevice;

        DefaultTextureList m_DefaultTextures;
        CustomTextureList  m_CheckboardTextures;
        CustomTextureList  m_DevTextures;
    };
} // namespace Ame::Rhi