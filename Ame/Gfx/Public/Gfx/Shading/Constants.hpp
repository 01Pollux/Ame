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
        static constexpr uint32_t c_MaxVertexAttributes = 4;
        static constexpr uint32_t c_MaxVertexStreams    = 1;

        MaterialVertexDesc()
        {
            Fill();
        }

    private:
        void Fill()
        {
            this->attributes   = m_Attributes;
            this->attributeNum = c_MaxVertexAttributes;
            this->streams      = m_Streams;
            this->streamNum    = c_MaxVertexStreams;
        }

        nri::VertexAttributeDesc m_Attributes[c_MaxVertexAttributes]{
            { .d3d{ "POSITION" }, .vk{ 0 }, .offset = offsetof(MaterialVertex, Position), .format = Rhi::ResourceFormat::RGB32_SFLOAT },
            { .d3d{ "NORMAL" }, .vk{ 1 }, .offset = offsetof(MaterialVertex, Normal), .format = Rhi::ResourceFormat::RGB32_SFLOAT },
            { .d3d{ "TANGENT" }, .vk{ 2 }, .offset = offsetof(MaterialVertex, Tangent), .format = Rhi::ResourceFormat::RGB32_SFLOAT },
            { .d3d{ "TEXCOORD" }, .vk{ 3 }, .offset = offsetof(MaterialVertex, TexCoord), .format = Rhi::ResourceFormat::RG32_SFLOAT }
        };

        nri::VertexStreamDesc m_Streams[c_MaxVertexStreams]{
            { .stride = sizeof(MaterialVertex) }
        };
    };
} // namespace Ame::Gfx::Shading
