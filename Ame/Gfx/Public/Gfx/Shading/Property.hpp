#pragma once

#include <Gfx/Shading/Core.hpp>

#include <Math/Vector.hpp>
#include <Math/Matrix.hpp>

#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/View.hpp>

#include <variant>

namespace Ame::Gfx::Shading
{
    enum class ResourceDataType : uint8_t
    {
        None,

        UInt,
        Int,
        UInt2,
        Int2,
        UInt3,
        Int3,
        UInt4,
        Int4,

        Float,
        Float2,
        Float3,
        Float4,

        Bool,
        Bool2,
        Bool3,
        Bool4,

        Matrix3x3,
        Matrix4x4,
    };

    enum class ResourceType : uint8_t
    {
        Scalar,
        Struct,

        Buffer,
        RWBuffer,

        StructuredBuffer,
        RWStructuredBuffer,

        Texture1D,
        Texture1DArray,
        RWTexture1D,
        RWTexture1DArray,

        Texture2D,
        Texture2DArray,
        Texture2DMS,
        Texture2DMSArray,
        RWTexture2D,
        RWTexture2DArray,

        Texture3D,
        RWTexture3D,

        TextureCube,
        TextureCubeArray,

        Sampler
    };

    //

    struct BufferResource
    {
        Ptr<Rhi::Buffer>       Buffer;
        Rhi::BufferViewDesc    ViewDesc;
        Ptr<Rhi::ResourceView> View;
    };

    struct TextureResource
    {
        Ptr<Rhi::Texture>      Texture;
        Rhi::TextureViewDesc   ViewDesc;
        Ptr<Rhi::ResourceView> View;
    };

    struct SamplerResource
    {
        Rhi::SamplerDesc       ViewDesc;
        Ptr<Rhi::ResourceView> View;
    };

    using RhiResourceType = std::variant<
        BufferResource,
        TextureResource,
        SamplerResource>;

    //

    template<typename Ty>
    struct ResourceMappable
    {
        static constexpr bool Enabled = false;
    };

#define AME_GFX_SHADING_RESOURCE_MAP(Ty)      \
    template<>                                \
    struct ResourceMappable<Ty>               \
    {                                         \
        static constexpr bool Enabled = true; \
    }

    AME_GFX_SHADING_RESOURCE_MAP(uint32_t);
    AME_GFX_SHADING_RESOURCE_MAP(int32_t);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector2U);
    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector2I);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector3U);
    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector3I);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector4U);
    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector4I);

    AME_GFX_SHADING_RESOURCE_MAP(float);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector2);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector3);
    AME_GFX_SHADING_RESOURCE_MAP(Math::Color3);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector4);
    AME_GFX_SHADING_RESOURCE_MAP(Math::Color4);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Matrix3x3);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Matrix4x4);

    //

#undef AME_GFX_SHADING_RESOURCE_MAP
} // namespace Ame::Gfx::Shading
