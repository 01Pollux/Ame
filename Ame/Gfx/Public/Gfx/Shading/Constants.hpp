#pragma once

#include <cstdint>
#include <Rhi/Descs/Pipeline.hpp>
#include <Ecs/Component/Renderable/VertexInput.hpp>

namespace Ame::Gfx::Shading
{
    using MaterialVertex = Ecs::Component::VertexInput;
    using Ecs_VSInput    = MaterialVertex;

    struct MaterialVertexDesc : Rhi::VertexInputDesc
    {
        static constexpr uint32_t MaxVertexAttributes = 4;

        MaterialVertexDesc()
        {
            Fill();
        }

    private:
        void Fill()
        {
            this->attributes   = m_Attributes;
            this->attributeNum = MaxVertexAttributes;
            this->streams      = m_Streams;
            this->streamNum    = 1;
        }

        nri::VertexAttributeDesc m_Attributes[MaxVertexAttributes]{
            { .d3d{ "POSITION" }, .vk{ 0 }, .offset = offsetof(MaterialVertex, Position), .format = Rhi::ResourceFormat::RGB32_SFLOAT },
            { .d3d{ "NORMAL" }, .vk{ 1 }, .offset = offsetof(MaterialVertex, Normal), .format = Rhi::ResourceFormat::RGB32_SFLOAT },
            { .d3d{ "TANGENT" }, .vk{ 2 }, .offset = offsetof(MaterialVertex, Tangent), .format = Rhi::ResourceFormat::RGB32_SFLOAT },
            { .d3d{ "TEXCOORD" }, .vk{ 3 }, .offset = offsetof(MaterialVertex, TexCoord), .format = Rhi::ResourceFormat::RG32_SFLOAT }
        };

        nri::VertexStreamDesc m_Streams[1]{
            { .stride = sizeof(MaterialVertex) }
        };
    };
} // namespace Ame::Gfx::Shading
