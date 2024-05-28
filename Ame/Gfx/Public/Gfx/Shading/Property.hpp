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

        Matrix2x2,
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
    struct ResourceMap;

#define AME_GFX_SHADING_RESOURCE_MAP(Ty, DT, T)                            \
    template<>                                                             \
    struct ResourceMap<Ty>                                                 \
    {                                                                      \
        static constexpr ResourceDataType DataType = ResourceDataType::DT; \
        static constexpr ResourceType     Type     = ResourceType::T;      \
    }

    AME_GFX_SHADING_RESOURCE_MAP(uint32_t, UInt, Scalar);
    AME_GFX_SHADING_RESOURCE_MAP(int32_t, Int, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector2U, UInt2, Scalar);
    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector2I, Int2, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector3U, UInt3, Scalar);
    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector3I, Int3, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector4U, UInt4, Scalar);
    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector4I, Int4, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(float, Float, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector2, Float2, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector3, Float3, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector4, Float4, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(bool, Bool, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector2B, Bool2, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector3B, Bool3, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Vector4B, Bool4, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Matrix2x2, Matrix2x2, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Matrix3x3, Matrix3x3, Scalar);

    AME_GFX_SHADING_RESOURCE_MAP(Math::Matrix4x4, Matrix4x4, Scalar);

    //

#undef AME_GFX_SHADING_RESOURCE_MAP
} // namespace Ame::Gfx::Shading

namespace Ame::Concepts::Gfx::Shading
{
    template<typename Ty>
    concept ResourceMappable = requires {
        {
            Ame::Gfx::Shading::ResourceMap<Ty>::DataType
        } -> std::convertible_to<Ame::Gfx::Shading::ResourceDataType>;
        {
            Ame::Gfx::Shading::ResourceMap<Ty>::Type
        } -> std::convertible_to<Ame::Gfx::Shading::ResourceType>;
    };
} // namespace Ame::Concepts::Gfx::Shading