#pragma once

#include <span>
#include <Rhi/Core.hpp>
#include <Math/Vector.hpp>

namespace Ame::Rhi
{
    using Vertex_Position = Math::Vector3;
    using Vertex_Normal   = Math::Vector3;
    using Vertex_TexCoord = Math::Vector2;
    using Vertex_Tangent  = Math::Vector3;

    struct VertexAttributeDesc
    {
        const char* TypeString;

        /// Attribute name {POSITION, NORMAL, TEXCOORD, TANGENT}.
        const char* Name;

        const char* SemanticName;

        /// The type of the attribute's components.
        Dg::VALUE_TYPE Type;

        /// The number of components in the attribute.
        uint8_t NumComponents = 0;

        // Offset to the attribute in the vertex buffer.
        uint8_t Offset = 0xFF;
    };

    enum class VertexInputFlags : uint8_t
    {
        None = 0,

        Position_Index = 0,
        Position       = 1 << Position_Index,

        Normal_Index = 1,
        Normal       = 1 << Normal_Index,

        TexCoord_Index = 2,
        TexCoord       = 1 << TexCoord_Index,

        Tangent_Index = 3,
        Tangent       = 1 << Tangent_Index,

        Count = 4
    };

    // clang-format off
    static constexpr VertexAttributeDesc c_InputVertexAttributes[] = {
        { "float3", "position",   "ATTRIB0",    Dg::VT_FLOAT32,  3, 0 },
        { "float3", "normal",     "ATTRIB1",    Dg::VT_FLOAT32,  3, sizeof(float[3])  },
        { "float2", "tex_coord",  "ATTRIB2",    Dg::VT_FLOAT32,  2, sizeof(float[3])  + sizeof(float[3])  },
        { "float3", "tangent",    "ATTRIB3",    Dg::VT_FLOAT32,  3, sizeof(float[3])  + sizeof(float[3])  + sizeof(float[2]) },
        {nullptr, nullptr, nullptr, Dg::VT_UNDEFINED, 0, sizeof(float[3])  + sizeof(float[3])  + sizeof(float[2]) + sizeof(float[3]) }
    };

    static constexpr VertexAttributeDesc c_OutputVertexAttributes[] = {
        { "float4", "screen_position",  "SV_POSITION",      Dg::VT_FLOAT32, 4 },
        { "float3", "world_position",   "WORLD_POSITION",   Dg::VT_FLOAT32, 3 },
        { "float3", "world_normal",     "WORLD_NORMAL",     Dg::VT_FLOAT32, 3 },
        { "float2", "tex_coord",        "TEXCOORD",         Dg::VT_FLOAT32, 2 },
        { "float3", "world_tangent",    "WORLD_TANGENT",    Dg::VT_FLOAT32, 3 },
    };
    // clang-format on
} // namespace Ame::Rhi
