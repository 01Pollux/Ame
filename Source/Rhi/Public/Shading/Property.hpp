#pragma once

#include <variant>

#include <Core/String.hpp>
#include <Math/Vector.hpp>
#include <Math/Matrix.hpp>
#include <Math/Colors.hpp>

namespace Ame::Rhi
{
    enum class MaterialDataType : uint8_t
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

    enum class MaterialResourceType : uint8_t
    {
        Scalar,
        Struct,
    };

    //

    template<typename Ty> struct MaterialDataMappable
    {
        static constexpr bool Enabled = false;
    };

#define AME_RHI_SHADING_RESOURCE_MAP(Ty)                                                                               \
    template<> struct MaterialDataMappable<Ty>                                                                         \
    {                                                                                                                  \
        static constexpr bool Enabled = true;                                                                          \
    }

    AME_RHI_SHADING_RESOURCE_MAP(uint32_t);
    AME_RHI_SHADING_RESOURCE_MAP(int32_t);

    AME_RHI_SHADING_RESOURCE_MAP(Math::Vector2U);
    AME_RHI_SHADING_RESOURCE_MAP(Math::Vector2I);

    AME_RHI_SHADING_RESOURCE_MAP(Math::Vector3U);
    AME_RHI_SHADING_RESOURCE_MAP(Math::Vector3I);

    AME_RHI_SHADING_RESOURCE_MAP(Math::Vector4U);
    AME_RHI_SHADING_RESOURCE_MAP(Math::Vector4I);

    AME_RHI_SHADING_RESOURCE_MAP(float);

    AME_RHI_SHADING_RESOURCE_MAP(Math::Vector2);

    AME_RHI_SHADING_RESOURCE_MAP(Math::Vector3);
    AME_RHI_SHADING_RESOURCE_MAP(Math::Color3);

    AME_RHI_SHADING_RESOURCE_MAP(Math::Vector4);
    AME_RHI_SHADING_RESOURCE_MAP(Math::Color4);

    AME_RHI_SHADING_RESOURCE_MAP(Math::Matrix3x3);

    AME_RHI_SHADING_RESOURCE_MAP(Math::Matrix4x4);

    //

#undef AME_RHI_SHADING_RESOURCE_MAP
} // namespace Ame::Rhi