#pragma once

#include <span>
#include <Shading/Core.hpp>

namespace Ame::Rhi
{
    struct VertexAttributeDesc
    {
        const char* TypeString;

        /// Attribute name {POSITION, NORMAL, TEXCOORD, TANGENT}.
        const char* Name;

        const char* SemanticName;

        /// The type of the attribute's components.
        Dg::VALUE_TYPE Type;

        /// Index of the vertex buffer that stores this attribute.
        uint8_t BufferId;

        /// The number of components in the attribute.
        uint8_t NumComponents = 0;
    };

    struct VertexInputConstants
    {
        uint32_t PositionOffset = std::numeric_limits<uint32_t>::max();
        uint32_t NormalOffset   = std::numeric_limits<uint32_t>::max();
        uint32_t TexCoordOffset = std::numeric_limits<uint32_t>::max();
        uint32_t TangentOffset  = std::numeric_limits<uint32_t>::max();
    };

    // clang-format off
    static constexpr VertexAttributeDesc c_OutputVertexAttributes[] = {
        { "float4", "screen_position",  "SV_POSITION",      Dg::VT_FLOAT32, 0xFF, 4 },
        { "float3", "world_position",   "WORLD_POSITION",   Dg::VT_FLOAT32, 0xFF, 3 },
        { "float3", "world_normal",     "WORLD_NORMAL",     Dg::VT_FLOAT32, 0xFF, 3 },
        { "float2", "tex_coord",        "TEXCOORD",         Dg::VT_FLOAT32, 0xFF, 2 },
        { "float3", "world_tangent",    "WORLD_TANGENT",    Dg::VT_FLOAT32, 0xFF, 3 },
    };
    // clang-format on
} // namespace Ame::Rhi
