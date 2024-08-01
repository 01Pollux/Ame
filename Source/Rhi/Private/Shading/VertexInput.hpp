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

    // clang-format off
    static constexpr VertexAttributeDesc c_InputVertexAttributes[] = {
        { "float3", "position",     "ATTRIB0", Dg::VT_FLOAT32, 0, 3 },
        { "float3", "normal",       "ATTRIB1", Dg::VT_FLOAT32, 1, 3 },
        { "float2", "tex_coord",    "ATTRIB2", Dg::VT_FLOAT32, 2, 2 },
        { "float3", "tangent",      "ATTRIB3", Dg::VT_FLOAT32, 3, 3 },
    };
    
    static constexpr VertexAttributeDesc c_OutputVertexAttributes[] = {
        { "float4", "screen_position",  "SV_POSITION",      Dg::VT_FLOAT32, 0xFF, 4 },
        { "float3", "world_position",   "WORLD_POSITION",   Dg::VT_FLOAT32, 0xFF, 3 },
        { "float3", "world_normal",     "WORLD_NORMAL",     Dg::VT_FLOAT32, 0xFF, 3 },
        { "float2", "tex_coord",        "TEXCOORD",         Dg::VT_FLOAT32, 0xFF, 2 },
        { "float3", "world_tangent",    "WORLD_TANGENT",    Dg::VT_FLOAT32, 0xFF, 3 },
    };
    // clang-format on

    //

    struct MaterialVertexDesc : Dg::InputLayoutDescX
    {
        MaterialVertexDesc(
            MaterialVertexInputFlags flags)
        {
            Fill(flags);
        }

    private:
        void Fill(
            MaterialVertexInputFlags flags)
        {
            for (uint32_t i = 0; i < std::to_underlying(MaterialVertexInputFlags::Count); i++)
            {
                const auto& attribute = c_InputVertexAttributes[i];

                bool isEnabled    = std::to_underlying(flags) & (1 << i);
                bool isNormalized = (attribute.Type == Dg::VT_UINT8 || attribute.Type == Dg::VT_INT8);

                Dg::LayoutElement element(
                    i,
                    attribute.BufferId,
                    attribute.NumComponents,
                    attribute.Type,
                    isNormalized,
                    0,
                    isEnabled ? Dg::LAYOUT_ELEMENT_AUTO_STRIDE : 0);
                Add(element);
            }
        }
    };
} // namespace Ame::Rhi
