#pragma once

#include <span>
#include <Shading/Core.hpp>

namespace Ame::Rhi
{
    struct VertexAttributeDesc
    {
        /// Attribute name ("POSITION", "NORMAL", "TEXCOORD_0", "TEXCOORD_1", "JOINTS_0", "WEIGHTS_0", etc.).
        const char* Name;

        /// The type of the attribute's components.
        Dg::VALUE_TYPE Type;

        /// Index of the vertex buffer that stores this attribute.
        uint8_t BufferId;

        /// The number of components in the attribute.
        uint8_t NumComponents = 0;
    };

    static constexpr VertexAttributeDesc c_VertexAttributes3D[] = {
        { "POSITION", Dg::VT_FLOAT32, 0, 3 },
        { "NORMAL", Dg::VT_FLOAT32, 1, 3 },
        { "TEXCOORD", Dg::VT_FLOAT32, 2, 2 },
        { "TANGENT", Dg::VT_FLOAT32, 3, 3 },
    };

    static constexpr VertexAttributeDesc c_VertexAttributes2D[] = {
        { "POSITION", Dg::VT_FLOAT32, 0, 2 },
        { "NORMAL", Dg::VT_FLOAT32, 1, 2 },
        { "TEXCOORD", Dg::VT_FLOAT32, 2, 2 },
        { "TANGENT", Dg::VT_FLOAT32, 3, 2 },
    };

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
            if (std::to_underlying(flags) & std::to_underlying(MaterialVertexInputFlags::Is2D))
            {
                Fill(flags, c_VertexAttributes2D);
            }
            else
            {
                Fill(flags, c_VertexAttributes3D);
            }
        }

        void Fill(
            MaterialVertexInputFlags                   flags,
            const std::span<const VertexAttributeDesc> attributes)
        {
            for (uint32_t i = 0; i < std::to_underlying(MaterialVertexInputFlags::Count); ++i)
            {
                const auto& attribute = attributes[i];

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
